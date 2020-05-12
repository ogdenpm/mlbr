// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Copyright (c) 2020 Mark Ogden
/*
    most of the OS specific functions are contained in this file
    They fall into the following groups
    file timestamps
    directory management
    filename management
    misc OS missing functions

*/

#include "mlbr.h"
#ifdef _WIN32
#include <Windows.h>
#include <ShlObj_core.h>
#else
#include <stdarg.h>
#endif


#ifndef _WIN32
// simulate windows _mkgmtime by seting env TZ to UTC, calling mktime & restoring TZ
time_t _mkgmtime(struct tm *tm) {
    time_t ret;     // simulate windows _mkgmtime 
    char *tz;

    tz = getenv("TZ");
    setenv("TZ", "", 1);
    tzset();
    ret = mktime(tm);
    if (tz)
        setenv("TZ", tz, 1);
    else
        unsetenv("TZ");
    tzset();
    return ret;
}
#endif

time_t getFileTime(FILE *fp) {
    struct stat buf;

    if (fstat(fileno(fp), &buf) != 0)
        return 0;
    return buf.st_mtime;
}

#ifdef _WIN32
// unfortunately utime does not appear to work with directories in windows
// so using Windows API instead, also allows create time to be set
uint64_t const unixDay0 = 116444736000000000;   // 1-Jan-1970 in FILETIME format
void setFileTime(char const *path, time_t ftime) {

    ULARGE_INTEGER hiresTime = { .QuadPart = unixDay0 + (uint64_t)ftime * 10000000ULL };
    FILETIME filetm = { hiresTime.LowPart, hiresTime.HighPart };

    // CreateFile needs wchar path name so convert
    int wsize = MultiByteToWideChar(CP_ACP, 0, path, -1, NULL, 0);
    wchar_t *wPath = xmalloc(wsize * sizeof(wchar_t));                
    MultiByteToWideChar(CP_ACP, 0, path, -1, wPath, wsize);

    // open the  file to allow update of the attributes
    HANDLE hFile = CreateFile(wPath,
        GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_WRITE_ATTRIBUTES,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL);
    xfree(wPath);
    if (hFile == INVALID_HANDLE_VALUE)              // quietly ignore if we could not open
        return;
    SetFileTime(hFile, &filetm, &filetm, &filetm);   // set time, quietly ignore errors
    CloseHandle(hFile);

}
#else
// simple version for none windows systems
void setFileTime(char const *path, time_t ftime) {

    struct utimbuf times = { ftime, ftime };
    utime(path, &times);

}
#endif

// utility to create dir if necessary
// returns false if name is already used but not a dir
// or cannot create dir otherwise returns true
bool safeMkdir(char const *dir) {
    struct stat info;
    if (stat(dir, &info) == 0)
        return (info.st_mode & S_IFDIR) != 0;
    return mkdir(dir, 0774) == 0;
}


// mkPath is mkdir with all intermediate directories
// created if necessary. Returns true if path created
#ifdef _MSC_VER
// under visual studio use the windows API
// convert dir to full path name first
bool recursiveMkdir(char const *dir) {
    struct stat info;
    // check if dir already exists also handles drive:\ which SHCreateDirectory fails on with access denied  !!

    if (stat(dir, &info) == 0 && (info.st_mode & S_IFDIR))
        return true;

    wchar_t *wdir = xmalloc((strlen(dir) + 1) * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, dir, -1, wdir, _MAX_PATH);

    wchar_t *wpath = _wfullpath(NULL, wdir, 0);
    xfree(wdir);
    if (!wpath)
        return false;
    int code = SHCreateDirectory(NULL, wpath);
    free(wpath);
    return code == ERROR_SUCCESS || code == ERROR_ALREADY_EXISTS;
}
#else
bool recursiveMkdir(char const *dir) {
    char *s;
    char *partdir = alloca(strlen(dir) + 3);        // allow for adding . on end of disk only spec for windows + terminating OSDIRSEP
    strcpy(partdir, dir);
#ifdef _WIN32
    if ((s = strchr(partdir, ':')) && !s[1] || (ISDIRSEP(s[1]) && !s[2])) {  // if disk only specified add '.' to check current dir on disk (else mkdir fails)
        strcat(s, ".");
        s = strchr(partdir, '\0');
    } else if (strncmp(partdir, "\\\\", 2) == 0) {
        if ((s = strchr(partdir + 2, '\\')) == NULL || *++s == '\0')       // skip past server name and check we have a host dir
            return false;                       // only server name
    } else
        s = ISDIRSEP(*partdir) ? partdir + 1 : partdir;     // leading / or \ for root
#else
    s = ISDIRSEP(*partdir) ? partdir + 1 : partdir;         // leading / for root
#endif
    bool ok = true;
    struct stat info;
    strcat(s, OSDIRSEP);                      // make sure last part is processed

    while (ok && (s = strpbrk(s, DIRSEP))) {       // a dir of root will skip while but return ok
        int c = *s;
        *s = '\0';                          // remove separator
        if (stat(partdir, &info) == 0)      // TODO: expand to handle symbolic links
            ok = info.st_mode & S_IFDIR;
        else
            ok = mkdir(partdir, 0774) == 0;
        *s = c;
        while (ISDIRSEP(*++s))
            ;
    }
    return ok;
}
#endif


// check the file specified by fn for for windows, reserved words, also define illegal chars
// note CPM 2.2 checks for the following illegal chars "=_:;<>?*" although many of DR's CPM apps also use [] for options
#ifndef _WIN32
static const char illegal[] = "/";
#define chkReserved(name)   false
#else
static const char illegal[] = "/\\|\"<>:?*";        // as a safety measure check for all illegal windows chars
static const char *res3[] = { "con", "prn", "aux", "nul", NULL };
static const char *res4[] = { "com", "lpt", NULL };

static bool chkReserved(char const *fn) {

    char const *s = strchr(fn, '.');                // find ext
    int flen = (int)(s ? (s - fn) : strlen(fn));    // length of filename part

    if ((flen == 4 && isdigit(fn[3])) || flen == 3) {       // reserved words are 3 chars or 4 where 4th is a digit
        for (char const **r = flen == 3 ? res3 : res4; *r; r++)
            if (strncmp(*r, fn, 3) == 0)
                return true;
    }
    return false;
}
#endif


// fin an os safe file name to store the file
// if the original filename is safe it will be used unless there is a clash with other names for example
//      upper/lower case clash for windows or for unix where the -k option is not specified
//          e.g. compressed files ABC.COM and abc.com would clash
//      the original file names clash with others in the container or the container name itself
//      the C implementations of squeeze would allow spaces in file names and multiple dots. As parseHeader removes
//      trailing spaces and dots, this may also potentially result in a name clash
//      parseHeader also silently truncates very long file names > 255 chars, although extremely unlikely this could
//      lead to name chashes
// if the file name is not os safe or clashes the follow is done to create a safe name
//      reserved file names in windows have an _ prefixed e.g. aux.c -> _aux.c
//      os reserved file name chars are  mapped to _ e.g. game/0.com -> game_0.com
//      if the above generated name clashes with other save names a (number) is added until no clash - number 0-99
//      e.g. hello.asm -> hello(1).asm
// Note, there is nothing in the specification of squeeze, crunch or lzh that would prevent directory paths
// however most are generated under cpm, hence the mapping of / (and \ for windows) to _

char const *uniqueName(char const *subDir, char const *fname) {
    // need to generate the unique name
    // alloc sufficient for subDir '/',  '_' prefix, possible "(nn)" suffix and '\0'
    char *savePath = xmalloc(strlen(subDir) + strlen(fname) + 7);

    strcpy(savePath, subDir);                   // prefix with subDir

    char *saveName = strchr(savePath, '\0');    // where we add the file name

    if (*savePath && !ISDIRSEP(saveName[-1]))   // make sure any non blank directory has dir separator at end
        strcat(saveName++, OSDIRSEP);

    if (chkReserved(fname))
        strcat(saveName++, "_");                // reserved names have _ prefix

    char *ext = strrchr(fname, '.');            // ext in original name if present
    if (!ext)
        ext = strchr(fname, '\0');              // no ext so point to end of name

    for (int i = 0; i < 100; i++) {
        if (i == 0)
            strcpy(saveName, fname);            // first try original name
        else                                    // for rest try (i) suffix (i = 1..99)
            sprintf(saveName, "%.*s(%d)%s", (int)(ext - fname), fname, i, ext);

        for (char *s = saveName; s = strpbrk(s, illegal); ) // map any illegal chars
            *s = '_';
        mapCase(saveName);                      // map to lower case if necessary directory element is not changed
        if (!chkClash(savePath))                // if unique return the full path
            return savePath;
    }
    fprintf(stderr, "Fatal: Too many name conflicts\n");
    exit(1);
}

// adds the src name to list of used names if the target directory is part of its path
// this prevents it being overwritten
// assumes targetDir is in cannocial format
// shouldn't clash as it should be the first name!!!
void protectSrc(const char *fname, const char *targetDir) {
    char *fullname = realpath(fname, NULL);     // convert to cannocial format
    if (fullname == 0)                          // shouldn't happen as we have already read file!!
        return;
    size_t dirlen = strlen(targetDir);          // check for targetDir/... as prefix
    // see if target is a prefix path for fname
    if (dirlen < strlen(fullname) && strncasecmp(targetDir, fullname, dirlen) == 0 && ISDIRSEP(fullname[dirlen])) {
        char *subDir = xmalloc(strlen(fullname) - dirlen - 1 + 1);  // possible prefix so add the part past targetDir
        strcpy(subDir, fullname + dirlen + 1);
        chkClash(subDir);       
    }
    free(fullname);
}


// return the name only part of a specified filename
char const *nameOnly(char const *fname) {
    char const *s;
#ifdef _WIN32
    while (s = strpbrk(fname, "\\/:"))
        fname = s + 1;
#else
    while (s = strchr(fname, '/'))
        fname = s + 1;
#endif
    return fname;
}

char const *makeFullPath(const char *targetDir, const char *fname) {
    const char *s = strchr(targetDir, '\0');
    if (s != targetDir && ISDIRSEP(s[-1]))              // avoid separator if not needed should only happen for root
        return concat(targetDir, fname, NULL);
    return concat(targetDir, OSDIRSEP, fname, NULL);
}

// gcc does not have strlwr
#ifndef _MSC_VER
char *strlwr(char *str) {
    for (char *s = str; *s; s++)
        *s = tolower(*s);
    return str;
}

// or _vscprintf

int _vscprintf(const char *fmt, va_list pargs)     {
    int retval;
    va_list argcopy;
    va_copy(argcopy, pargs);
    retval = vsnprintf(NULL, 0, fmt, argcopy);
    va_end(argcopy);
    return retval;

}
#endif