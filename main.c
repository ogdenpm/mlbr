/* mlbr - extract .lbr archives and decompress Squeeze, Crunch (v1 & v2)
 *        and Cr-Lzh(v1 & v2) files.
 *	Comments and date stamps are supported as is conversion to .zip file
 *	Copyright (C) - 2020-2023 Mark Ogden
 *
 * main.c - top level option and file processing
 *
 * NOET: Elements of the code have been derived from public shared
 * source code and documentation.
 * The source files note the owning copyright holders where known
 * 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "mlbr.h"
#include <stdarg.h>
#include "showVersion.h"

bool keepCase         = false;
bool ignoreCorrupt    = false;
bool ignoreCrc        = false;
bool srcDstSame       = false;
int flags             = 0;
char const *targetDir = ".";

char *mapCase(char *s) {
    return keepCase ? s : strlwr(s);
}

char const *methodName(content_t *content) {
    switch (content->type) {
    case Squeezed:
        return "Squeezed";
    case Crunched:
        return "Crunched";
    case CrunchV1:
        return "CrunchV1";
    case CrunchV2:
        return "CrunchV2";
    case CrLzh:
        return "Cr-Lzh";
    case CrLzhV1:
        return "Cr-LzhV1";
    case CrLzhV2:
        return "Cr-LzhV2";
    case Library:
        return "Library";
    case Stored:
        return "Stored";
    case Skipped:
        return "Skipped";
    case Missing:
        return "No Data";
    default:
        return "Unknown";
    }
}

int getMethod(content_t *content) {
    switch (inU16(content)) {
    case 0xfd76:
        return CrLzh;
    case 0xfe76:
        return Crunched;
    case 0xff76:
        return Squeezed;
    case 0x2000:
        if (content->in.bufSize >= 128 && memcmp(content->in.buf, "\0           \0\0", 14) == 0) {
            return Library;
        }
        break;
    case EOF:
        if (content->in.bufSize < 0) {
            return Missing;
        }
    }
    return Stored;
}

// process a file / library
// returns the number of non skipped / missing files available
int processFile(content_t *content, int flags, int depth) {
    int result = 0;
    switch (content->type = getMethod(content)) {
    case Squeezed:
        result = unsqueeze(content);
        break;
    case Crunched:
        result = uncrunch(content);
        break;
    case CrLzh:
        result = uncrLzh(content);
        break;
    case Library:
        if ((depth == 0 || (flags & RECURSE)) && parseLbr(content)) {
            int valid = 0;
            for (content_t *p = content->lbrHead; p; p = p->next) {
                valid += processFile(p, flags, depth + 1);
            }
            if (content->out.fdate > 0) {
                content->in.fdate = content->out.fdate; // fixup the actual lbr date but not if invalid
            }
            setStoreFile(content); // keep list happy with sensible filename & expected length
            return valid;
        } else {
            content->type = Stored; // not library or nested .lbr with no recurse
        }
        break;
    case Stored:
        break;
    case Missing:
        setStoreFile(content); // keep list happy with sensible filename & expected length
        return 0;
    }
    char const *msg;
    if (content->type != Stored) {
        switch (result) {
        case GOOD:
            if (flags & NOEXPAND) {
                msg = "is valid";
            } else {
                return 1;
            }
            break;
        case BADCRC:
            msg = "has invalid CRC";
            break;
        case CORRUPT:
            msg = "is corrupt";
            break;
        default:
            msg = "invalid header";
            break;
        }

        bool ignore   = (result == BADCRC && ignoreCrc) || (result == CORRUPT && ignoreCorrupt);
        bool lbrCrcOk = depth && !(content->status & (F_BADCRC | F_NOCRC | F_TRUNCATED));
        logErr(content, "!! %s [%s %s] %s, %s%s\n", content->in.fname, methodName(content),
               content->out.fname, msg, ignore ? "ignoring error" : "processing as normal file",
               lbrCrcOk ? ", LBR CRC Ok" : "");
        if (ignore) {
            return 1;
        }
    }
    setStoreFile(content);
    if ((!srcDstSame || depth) && (!(content->status & F_TRUNCATED) || (flags & FORCE))) {
        content->type = Stored;
        return 1;
    }
    content->type = Skipped;
    return 0;
}

void displayDate(const time_t date) {
    struct tm const *timeptr = gmtime(&date);
    printf("%04d-%02d-%02d %02d:%02d", 1900 + timeptr->tm_year, timeptr->tm_mon + 1,
           timeptr->tm_mday, timeptr->tm_hour, timeptr->tm_min);
}

void list(content_t *content, time_t defDate, int depth) {
    for (content_t *p = content; p; p = p->next) {
        if (p->msg) {
            printf("%s", p->msg);
        }
        printf("%*s", depth * 2, "");
        printf("%-*s %7ld %-9s", 19 - depth * 2, p->out.fname, p->out.pos, methodName(p));
        switch (p->type) {
        case Crunched:
        case Squeezed:
        case CrLzh:
        case CrunchV1:
        case CrunchV2:
        case CrLzhV1:
        case CrLzhV2:
            printf(" (%-12s %7ld) ", p->in.fname, p->in.bufSize);
            break;
        default:
            printf("%-24c", ' ');
            break;
        }
        putchar((p->status & F_BADCRC) ? 'X' : (p->status & F_NOCRC) ? '-' : ' ');
        putchar(' ');
        if (p->out.fdate) {
            if (0 < p->out.fdate && p->out.fdate <= defDate)
                displayDate(p->out.fdate);
            else {
                if (p->type == Library) {
                    displayDate(defDate);
                    putchar('*');
                } else
                    printf("< invalid date >");
                p->out.fdate = defDate;
            }
        } else {
            if (p->type == Library) {
                displayDate(defDate);
                putchar('=');
            } else
                printf("<no date record>");
            p->out.fdate = defDate;
        }
        if (p->comment) {
            printf(" %s", p->comment);
        }
        putchar('\n');
        if (p->type == Library) {
            if (p->lbrHead) {
                list(p->lbrHead, p->out.fdate, depth + 1);
            } else {
                printf(" -- empty library --\n");
            }
        }
    }
}

void usage(char const *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);
    fprintf(stderr,
            "\n"
            "Usage: mlbr -v | -V | -h | [-x | -d | -z]  [-D dir] [-f] [-i] [-k] [-n] [-r] [--] file+\n"
            "   -v / -V show version information and exit\n"
            "   -h  show this help and exit\n"
            "   -x  extract to directory\n"
            "   -d  extract lbr to sub directory {name} - see below\n"
            "   -z  convert to zip file {name}.zip\n"
            "   -D  override target directory\n"
            "   -f  forces write of skipped library content\n"
            "   -i  ignore crc errors\n"
            "   -I  ignore crc errors and corrupt decompression\n"
            "   -k  keep original case of file names (default is to lower case)\n"
            "   -n  don't expand compressed files\n"
            "   -r  recursively extract lbr, creates nested sub directories for -d or -z\n"
            "   --  terminates args to support files with a leading -\n\n"

            " file+ one or more lbr, squeezed, crunched or crLzhed files\n"
            "       names can include * or ? wildcard characters\n"
            "{name} is file with leading directory and extent removed\n"
            "\n"
            " Listing of file details, including validation checks is always done\n"
            " The -x or -d options allow files to be extracted\n\n"
            " When files contain comments or are renamed to avoid conflicts\n"
            " the details are written to a file {name}.info\n");
    va_end(args);
    exit(*fmt ? 1 : 0);
}

// expands one file
// cwd and targetDir should be in cannocial form
bool expandFile(char const *fname, char const *targetDir, int flags) {
    bool ok = true;
    file_t *file;
    content_t *content;

    printf("%s:", fname);
    if (!(file = loadFile(fname))) {
        return false;
    }
    putchar('\n');
    content     = makeDescriptor(file, file->fname, file->buf, file->bufSize);

    int saveCnt = processFile(content, flags, 0);

    list(content, file->fdate, 0);
    putchar('\n'); // space from next block of info
    if (saveCnt != 0 && (flags & SAVEMASK)) {
        if (flags & (EXTRACT | SUBDIR)) {
            mkOsNames(content, "", flags);
            ok = saveContent(content, targetDir);
        } else if (flags & ZIP) {
            char const *zipFile = replaceExt(file->fname, ".zip");
            zipFile             = uniqueName("", zipFile);
            mkOsNames(content, "", flags);
            ok = saveZip(content, targetDir, zipFile);
        }
        putchar('\n'); // space from next block of info
    }

    freeAllDescriptors(content);
    sFree(); // clear all of the strings allocated
    unloadFile(file);
    return true;
}

int parseOptions(int argc, char **argv) {
    int arg;
    int saveOpt = 0;

    for (arg = 1; arg < argc && argv[arg][0] == '-'; arg++) {
        if (strlen(argv[arg]) != 2) {
            usage("Invalid option %s\n", argv[arg]);
        }
        if (strcmp(argv[arg], "--") == 0) {
            arg++;
            break;
        }
        switch (argv[arg][1]) {
        case 'x':
            flags |= EXTRACT;
            saveOpt++;
            break;
        case 'd':
            flags |= SUBDIR;
            saveOpt++;
            break;
        case 'z':
            flags |= ZIP;
            saveOpt++;
            break;
        case 'f':
            flags |= FORCE;
            break;
        case 'I':
            ignoreCorrupt = true; // fall through to also ignoreCrc
        case 'i':
            ignoreCrc = true;
            break;
        case 'k':
            keepCase = true;
            break;
        case 'n':
            flags |= NOEXPAND;
        case 'r':
            flags |= RECURSE;
            break;
        case 'D':
            if (++arg < argc) {
                targetDir = argv[arg];
            } else {
                usage("Missing directory for -D option\n");
            }
            break;
        default:
            usage("Invalid option %s\n", argv[arg]);
        }
    }
    if (saveOpt > 1) {
        usage("only one of -x, -d and -z allowed\n");
    }
    return arg;
}
int main(int argc, char **argv) {
    bool ok = true;

    CHK_SHOW_VERSION(argc, argv);
    if (argc == 2 && strcmp(argv[1], "-h") == 0)
        usage("");



    int arg = parseOptions(argc, argv);

    if (arg >= argc) {
        usage("No file specified\n");
    }

    char *cwd = realpath(".", NULL); // used to restore to original directory
    if (!cwd) {
        fprintf(stderr, "cannot resolve current working directory\n");
        exit(1);
    }
    char *fullTargetDir;

    if (strcmp(targetDir, ".") != 0 && (flags & SAVEMASK)) { // create the target directory
        if (!mkPath(targetDir)) {
            usage("cannot create directory %s\n", targetDir);
        }
        if ((fullTargetDir = realpath(targetDir, NULL)) == NULL) {
            fprintf(stderr, "cannot resolve %s\n", targetDir);
            exit(1);
        }
    } else {
        fullTargetDir = cwd;
    }

    srcDstSame = nameCmp(fullTargetDir, cwd) == 0;

    if (flags & SAVEMASK) { // will be saving to protect all of the source files if necessary
        for (int i = arg; i < argc; i++) {
            protectSrc(argv[i], fullTargetDir);
        }
    }

    for (; arg < argc; arg++) {
        ok = ok && expandFile(argv[arg], fullTargetDir, flags);
    }

    if (fullTargetDir != cwd) {
        free(fullTargetDir);
    }
    free(cwd);
#if _DEBUG
    dumpNames();
#endif
    freeHashTable();
    return ok ? 0 : 1;
}
