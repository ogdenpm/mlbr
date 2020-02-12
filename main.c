// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// Copyright (c) 2020 Mark Ogden 
#include "mlbr.h"
#include <stdarg.h>


static void displayDate(const time_t date);
bool kopt;

char *mapCase(char *s) {
    return kopt ? s : strlwr(s);
}

char const *methodName(content_t *content) {
    switch (content->type) {
    case squeezed: return "Squeezed";
    case crunched: return "Crunched";
    case crunchV1: return "CrunchV1";
    case crunchV2: return "CrunchV2";
    case crLzh:    return "Cr-Lzh";
    case crLzhV1:  return "Cr-LzhV1";
    case crLzhV2:  return "Cr-LzhV2";
    case library:  return "Library";
    case stored:   return "Stored";
    case skipped:  return "Skipped";
    case missing:  return "No Data";
    default:       return "Unknown";
    }
}

int getMethod(content_t *content) {
    switch (inU16(content)) {
    case 0xfd76: return crLzh;
    case 0xfe76: return crunched;
    case 0xff76: return squeezed;
    case 0x2000:
        if (!(content->status & INCONTAINER) && content->in.bufSize >= 128 && memcmp(content->in.buf, "\0           \0\0", 14) == 0)
            return library;
        break;
    case EOF:
        if (content->in.bufSize < 0)
            return missing;
    }
    return stored;

}


// process a file / library
// returns the number of non skipped / missing files available
int processFile(content_t *content, bool force) {

    switch (content->type = getMethod(content)) {
    case squeezed:
        if (unsqueeze(content))
            return 1;
        break;
    case crunched:
        if (uncrunch(content))
            return 1;
        break;
    case crLzh:
        if (uncrLzh(content))
            return 1;
        break;
    case library:
        if (parseLbr(content)) {
            int valid = 0;
            for (content_t *p = content->next; p; p = p->next)
                valid += processFile(p, force);
            setStoreFile(content);      // keep list happy with sensible filename & expected length
            return valid;
        }
        break;
    case missing:
        setStoreFile(content);      // keep list happy with sensible filename & expected length
        return 0;
    case stored:
        break;

    }
    if (content->type != stored) {
        printf("!! %s [%s", content->in.fname, methodName(content));
        if (content->out.fname)
            printf(" %s", content->out.fname);
        printf("] is corrupt, processing as normal file");
        if ((content->status & (INCONTAINER | BADCRC | NOCRC | TRUNCATED)) == INCONTAINER)
            printf(", LBR CRC is ok");
        putchar('\n');
    }
    setStoreFile(content);
    if ((content->status & INCONTAINER) && (force || !(content->status & TRUNCATED))) {
        content->type = stored;
        return 1;
    }
    content->type = skipped;
    return 0;
}



static void displayDate(const time_t date) {
        struct tm const *timeptr = gmtime(&date);
        printf("%04d-%02d-%02d %02d:%02d", 1900 + timeptr->tm_year, timeptr->tm_mon + 1, timeptr->tm_mday, timeptr->tm_hour, timeptr->tm_min);
}

void list(content_t *content) {
    putchar('\n');          // separate from any error messages
    for (content_t *p = content; p; p = p->next) {
        printf("%-12s %7ld %-9s", p->out.fname, p->out.pos, methodName(p));
        switch (p->type) {
        case crunched: case squeezed: case crLzh:
        case crunchV1: case crunchV2: case crLzhV1: case crLzhV2:
            printf(" (%-12s %7ld) ", p->in.fname, p->in.bufSize);
            break;
        default:
            printf("%-24c", ' '); break;
        }
        putchar((p->status & BADCRC) ? 'X' : (p->status & NOCRC) ? '-' : ' ');
        putchar(' ');
        if (p->out.fdate)
            displayDate(p->out.fdate);
        else {
            printf("    <no date>   ");
            p->out.fdate = content->in.fdate;
        }
        if (p->comment)
            printf(" %s", p->comment);
        if (p->type == library && p->next == NULL)
            printf("\n -- empty library --");
        putchar('\n');
    }
    putchar('\n');
}


void usage(char const *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "mlbr 1.0 - copyright (c) 2020 Mark Ogden.\n\n** ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n"
        "Usage: mlbr [-x | -d | -z]  [-D dir]] [-f] [-k] [--] file+\n"

        "   -x  extract to directory\n"
        "   -d  extract to sub directory {name} - see below\n"
        "   -z  convert to zip file {name}.zip\n"
        "   -D  override target directory\n"
        "   -f  forces write of skipped library content\n"
        "   -k  retains case of original file names (default is lower case)\n"
        "   --  terminates args to support files with a leading -\n\n"

        " file+ one or more lbr, squeezed, crunched or crLzhed files\n"
#ifdef _WIN32
        "       names can include * or ? wildcard characters\n"
#endif
        "{name} is file with leading directory and extent removed\n"
        "\n"
        " Listing of file details, including validation checks is always done\n"
        " The -x or -d options allow files to be extracted\n\n"
        " When files contain comments or are renamed to avoid conflicts\n"
        " the details are written to a file {name}.info\n"
    );
    exit(1);
}



char *resolveDir(char const *dir, char const *subDir) {
    char *path = xmalloc(strlen(dir) + (subDir ? strlen(subDir) : 0) + 2);

    strcpy(path, dir);
    if (subDir) {
        char *s = strchr(path, '\0');           // end of path so far
        if (s != path && !ISDIRSEP(s[-1]))      // need to add separator
            *s++ = '/';
        strcpy(s, subDir);                      // append the subDir
    }
    if (path[0] == 0)
        return path;
    if (rmkdir(path))
       return path;
    printf("problems creating target directory %s\n", path);
    free(path);
    return NULL;
}

int main(int argc, char **argv) {
    file_t *file;
    content_t *content;
    bool xopt = false;
    bool dopt = false;
    bool fopt = false;
    bool zopt = false;
    int ok = 0;
    char const *userDir = "";
    char *stem = NULL;
    char *dir = NULL;

    int arg;
    for (arg = 1; arg < argc && argv[arg][0] == '-'; arg++) {
        if (strlen(argv[arg]) != 2)
            usage("Invalid option %s\n", argv[arg]);
        if (strcmp(argv[arg], "--") == 0) {
            arg++;
            break;
        }
        switch (argv[arg][1]) {
        case 'x':
            xopt = true;
            break;
        case 'd':
            dopt = true;
            break;
        case 'z':
            zopt = true;
            break;
        case 'f':
            fopt = true;
            break;
        case 'k':
            kopt = true;
            break;
        case 'D':
            if (++arg < argc)
                userDir = argv[arg];
            else
                usage("Missing directory for -D option\n");
            break;
        default:
            usage("Invalid option %s\n", argv[arg]);
        }
    }
    if (xopt + dopt + zopt > 1)
        usage("Only one of -x, -d and -z allowed\n");

    if (arg >= argc)
        usage("No file specified\n");

    for (; arg < argc; arg++) {
        printf("%s:\n", argv[arg]);
        file = loadFile(argv[arg]);
        if (!file)
            continue;
        content = makeDescriptor(file, file->fname, file->buf, file->bufSize);
        int saveCnt = processFile(content, fopt);

        list(content);
        if (saveCnt != 0) {
            if (dopt)
                stem = replaceExt(file->fname, "");
            else if (zopt)
                stem = replaceExt(file->fname, ".zip");
            else
                stem = NULL;
            if (xopt || dopt) {
                if (dir = resolveDir(userDir, stem)) {
                    mkOsNames(content);
                    ok &= saveContent(content, dir);
                    if (dopt)
                        setFileTime(dir, file->fdate);
                    free(dir);
                } else
                    ok = false;
            } else if (zopt) {
                if (dir = resolveDir(userDir, "")) {
                    mkOsNames(content);
                    saveZipContent(content, dir, stem);
                    free(dir);
                } else
                    ok = false;
            }
            if (stem)
                free(stem);
        }
        freeAllDescriptors(content);

        unloadFile(file);
    }
    return ok ? 0 : 1;
}
