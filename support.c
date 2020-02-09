// Copyright (c) 2020 Mark Ogden 
#include "mlbr.h"
uint16_t crc16(uint8_t const *data, long len) {
    uint16_t x;
    uint16_t crc = 0;

    while (len-- > 0) {
        x = (crc >> 8) ^ *data++;
        x ^= x >> 4;
        crc = (crc << 8) ^ (x << 12) ^ (x << 5) ^ x;
    }
    return crc;
}

uint16_t crc(uint8_t const *data, long len) {
    uint16_t crc = 0;
    while (len-- > 0)
        crc += *data++;
    return crc;
}

time_t getFileTime(FILE *fp) {
    struct stat buf;

    if (fstat(fileno(fp), &buf) != 0)
        return 0;
    return buf.st_mtime;
}

void setFileTime(char const *path, time_t ftime) {
    struct utimbuf times = { ftime, ftime };
    utime(path, &times);
}

int u16At(uint8_t const *buf, long offset) {
    return buf[offset] + buf[offset + 1] * 256;
}



/*
    Crunch time is stored in 3 fields
    createTime  0-4
    accessTime  5-9
    modifyTime  10-14

    each field is encoded in bcd
    year     - assume < 78 is 2000 onwards
    month
    day
    hour
    minute
    0xff -> 0
*/
static int bcd2Int(uint8_t n) {
    if (n == 0xff)
        return 0;
    if (n > 0x99 || n % 16 > 9)
        return -1;
    return (n / 16) * 10 + n % 16;
}

time_t getCrunchTime(uint8_t const *dateStamp) {
    struct tm timebuf;
    static struct { uint8_t low, high; } check[] = { {0, 99}, {1, 12}, {1, 31}, {0, 23}, {0, 59} };
    int16_t dateValue[5];

    if (bcd2Int(dateStamp[11]) > 0)     // is modify specified
        dateStamp += 10;                // is so use it as the base

    for (int i = 0; i < 5; i++)
        if ((dateValue[i] = bcd2Int(dateStamp[i])) < check[i].low || dateValue[i] > check[i].high)
            return 0;
   
    timebuf.tm_year = dateValue[0] + (dateValue[0] < 78 ? 100 : 0);
    timebuf.tm_mon = dateValue[1] - 1;
    timebuf.tm_mday = dateValue[2];
    timebuf.tm_hour = dateValue[3];
    timebuf.tm_min = dateValue[4];
    timebuf.tm_sec = 0;
    timebuf.tm_isdst = -1;
    return mktime(&timebuf);
}

time_t cpmToOsTime(unsigned cpmDay, unsigned timeInSecs) {
    static time_t timeZero;

    if (timeZero == 0) {                            // get CP/M base time for this system
        struct tm timebuf = { 0, 0, 0, 31, 11, 77, -1 };

        timeZero = mktime(&timebuf);
    }


    if (cpmDay || timeInSecs) {
        time_t osTime = timeZero + cpmDay * (24 * 3600) + timeInSecs;
        // see if the os thinks DST was in effect, if so remove the DST hour
        struct tm const *timeptr = localtime(&osTime);
        return timeptr->tm_isdst == 1 ? osTime - 3600 : osTime;
    } else
        return 0;
}

// check the file specified by fn for illegal chars and for windows, reserved words
// note CPM 2.2 checks for the following illegal chars "=_:;<>?*" although many of DR's CPM apps also use [] for options
#ifndef _WIN32
static const char illegal[] = "/";
#else
static const char illegal[] = "/\\|\"<>:?*";        // as a safety measure check for all illegal windows chars
static const char *res3[] = { "con", "prn", "aux", "nul", NULL };
static const char *res4[] = { "com", "lpt", NULL };
#endif

int chkName(char const *fn) {
    char const *s;
    int result = 0;

    if (strpbrk(fn, illegal))
        result |= ILLEGALCHAR;
#ifdef _WIN32
    (s = strchr(fn, '.')) || (s = strchr(fn, '\0'));        // s -> extent or if none end of string
    int flen = (int)(s - fn);                               // length of filename part

    if ((flen == 4 && isdigit(fn[3])) || flen == 3) {       // reserved words are 3 chars or 4 where 4th is a digit
        for (char const **r = flen == 3 ? res3 : res4; *r; r++)
            if (strncmp(*r, fn, 3) == 0) {
                result |= RESERVED;
                break;
            }
    }
#endif
    return result;
}


bool nameClash(content_t *content, char const *name) {
    for (content_t *p = content; p; p = p->next) {
        if (p->saveName && p->saveName != name && nameCmp(p->saveName, name) == 0)
            return true;
    }
    return false;
}



// identify an os safe file name to store the file
// if the original filename is safe it will be used unless there is a clash with other names for example
//      upper/lower case clash for unix created files as names are mapped to lower case before saving
//          e.g. compressed files ABC.COM and abc.com would clash
//      the original file names clash with others in the container or the container name itself
//      although unlikely very very long names with directory paths (> 255 chars) are truncated
//      the C implementations of squeeze would allow spaces in file names and multiple dots. As parseHeader removes
//      trailing spaces and dots, this may also potentially result in a name clash
// if the file name is not os safe or clashes the follow is done to create a safe name
//      reserved file names in windows have an _ prefixed e.g. aux.c -> _aux.c
//      os reserved file name chars are  mapped to _ e.g. game/0.com -> game_0.com
//      if the above generated name clashes with other save names a (number) is added until no clash - number 0-99
//      e.g. hello.asm -> hello(1).asm
// Note, there is nothing in the specification of squeeze, crunch or lzh that would prevent directory paths
// however most are generated under cpm, hence the mapping of / (and \ for windows) to _
// if files are mapped then a file origname._map_ is writen containing the details

content_t *addMappingFile(content_t *content) {
    char *mapname = xmalloc(strlen(content->in.fname) + 6);                   // worst case is origname + .info when original has no ext
    strcpy(mapname, content->in.fname);
    char *s;
    if (s = strrchr(mapname, '.'))                                          // remove any existing extent;
        *s = 0;
    strcat(mapname, ".info");

    content_t *mapcontent = makeDescriptor(&content->in, mapname, NULL, 0);
    mapcontent->out = mapcontent->in;
    mapcontent->type = mapping;
    mapcontent->next = content->next;                                       // add to list & set type
    content->next = mapcontent;
    
    free(mapname);
    return mapcontent;
}

void mkOsNames(content_t *content) {
    int nameFlags;
    // no longer needed
    // if original file name is safe for OS and not duplicate, then use it
    content_t *mapcontent = addMappingFile(content);

    for (content_t *p = content; p; p = p->next) {
        if ((nameFlags = chkName(p->out.fname)) == 0) {                       // assigned name is ok for OS so use it
            if (nameClash(content, p->saveName = p->out.fname))
                p->saveName = NULL;                                           // expanded file has same name clashes with container or other file
        }  else
            p->status |= nameFlags;
        if (p->comment) {
            outStr(mapcontent, p->out.fname);
            outStr(mapcontent, " ");
            outStr(mapcontent, p->comment);
            outStr(mapcontent, "\r\n");
        }
    }

    // if we still don't have an OS save name then generate a safe one
    for (content_t *p = content; p; p = p->next) {
        if (p->saveName || p->type == skipped)
            continue;
        // no need to generate safe name for skipped files
        p->saveName = xmalloc(strlen(p->out.fname) + 6);                // might grow to include _ prefix and (nn) suffix to name part
        char *ext;
        (ext = strrchr(p->out.fname, '.')) || (ext = strchr(p->out.fname, '\0'));       // ext in original name if present


        int i;
        for (i = 0; i <= 100; i++) {
            char suffix[5] = "";
            if (i)
                sprintf(suffix, "(%d)", i - 1);

            sprintf(p->saveName, "%s%.*s%s%s", (p->status & RESERVED) ? "_" : "", (int)(ext - p->out.fname), p->out.fname, suffix, ext);

            if (p->status & ILLEGALCHAR)      // map any illegal chars
                for (char *s = p->saveName; s = strpbrk(s, illegal); )
                    *s = '_';
            mapCase(p->saveName);
            if (!nameClash(content, p->saveName))
                break;
        }
        if (i > 100) {
            fprintf(stderr, "Fatal: Too many name OS conflicts\n");
            exit(1);
        }
        outStr(mapcontent, p->saveName);
        outStr(mapcontent, " <=> ");
        outStr(mapcontent, p->out.fname);
        outStr(mapcontent, "\r\n");
    }
    if (mapcontent->out.pos == 0)
        mapcontent->type = skipped;
}

void *xmalloc(size_t size) {
    void *p = malloc(size);
    if (p)
        return p;
    fprintf(stderr, "Fatal Error: Out of memory\n");
    exit(1);
}

void *xcalloc(size_t count, size_t size) {
    void *p = calloc(count, size);
    if (p)
        return p;
    fprintf(stderr, "Fatal Error: Out of memory\n");
    exit(1);
}

void *xrealloc(void *p, size_t size) {
    p = realloc(p, size);
    if (p)
        return p;
    fprintf(stderr, "Fatal Error: Out of memory\n");
    exit(1);
}

char *xstrdup(char const *s) {
    char *dup = xmalloc(strlen(s) + 1);
    strcpy(dup, s);
    return dup;
}

bool parseHeader(content_t *content) {
    uint8_t buf[MAX_HEADER + 4];        // worst case is . at buf[MAX_HEADER - 1], we may set null at buf[MAX_HEADER + 3]
    int c;
    int len;

    inSeek(content, content->type == squeezed ? 4 : 2);
    for (len = 0; (c = inU8(content)) > 0; len++)
        if (len < MAX_HEADER)
            buf[len] = c;
    if (c < 0)                          // no null
        return false;
    buf[len <= MAX_HEADER ? len : MAX_HEADER] = 0;  // terminate buffer

    if (content->type != squeezed) {
        uint8_t *stamp;
        if ((stamp = strchr(buf, '.')) && strlen(stamp) > 4) { // if .xxx allow comment/date
            stamp += 4;                                        // standard says .xxx but be flexible
            uint8_t *comment = strchr(stamp, '[');
            if (comment == stamp)
                if (stamp = strchr(comment, ']'))   // allow for date to follow comment
                    stamp++;
                else
                    stamp = strchr(comment, 1);     // or explicit tag

            if (stamp) {                    // stamp -> date stamp if present
                if (*stamp == 1 || *stamp == 'D')            // explict tag
                    *stamp++ = 0;           // if after comment mark end of comment
                if (strlen(stamp) >= 15) {   // comment start is not a valid date field so getCrunchTime will return 0 if there is overlap
                    time_t fdate = getCrunchTime(stamp);
                    if (fdate)
                        content->out.fdate =fdate;
                }
                *stamp = 0;                 // make sure any comment is terminated
            }
            if (comment) {
                content->comment = xstrdup(comment);
                *comment = 0;
            }
        }
    }
    uint8_t *s;
    // although chars < ' ' are not meant to be in the file name
    // one example I have seen has \b chars after the 3rd ext char
    // so some extra code to truncate ext if it contains <= ' '
    if (s = strchr(buf, '.')) {
        while (*++s && (*s & 0x7f) >= ' ')
            ;
        *s = 0;
    }
    for (s = buf; *s; s++)
        if ((*s &= 0x7f) < ' ')    // illegal name so assume not compressed file
            return false;
    if (--s >= buf && *s == '.')                 // remove null ext
        *s = 0;
    content->out.fname = mapCase(xstrdup(buf));
    if (len > MAX_HEADER)
        printf("Warning: %s header truncated\n", content->in.fname);
    return true;
}


bool rmkdir(char const *dir) {
    char const *s;

#ifdef _WIN32
    if (s = strchr(dir, ':'))                   // disk also allows for unc access
        ;
    else if (strncmp(dir, "\\\\", 2) == 0) {
        if ((s = strchr(dir + 2, '\\')) == NULL || *++s == '\0')       // skip past server name and check we have a host dir
            return false;                       // only server name
    } else
        s = ISDIRSEP(*dir) ? dir + 1 : dir;     // leading / or \ for root
#else
    s = ISDIRSEP(*dir) ? dir : dir + 1;         // leading / for root
#endif
    bool ok = true;
    char *path = xcalloc(strlen(dir) + 2, 1);   // allow for adding a . on end of disk only spec for windows
    struct stat info;

    while (ok && *s) {                          // a dir of root will skip while but return ok
        char const *t = strpbrk(s, DIRSEP);
        s = t ? t : strchr(s, '\0');
        memcpy(path, dir, s - dir);             // try subpath
#ifdef _WIN32
        if (path[s - dir - 1] == ':')           // disk only given
            path[s - dir] = '.';                // make it look at the directory so stat works
#endif
        if (stat(path, &info) == 0)
            ok = info.st_mode & S_IFDIR;
        else
            ok = mkdir(path, 0774) == 0;
        while (ISDIRSEP(*s))                    // skip potentially multiple dir separators
            s++;
    }
    free(path);
    return ok;
}

#ifndef _MSC_VER
char *strlwr(char *str) {
    for (char *s = str; *s; s++)
        *s = tolower(*s);
    return str;
}
#endif