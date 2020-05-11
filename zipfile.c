// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "mlbr.h"
#define MINIZ_HEADER_FILE_ONLY
#include "miniz.h"
#include "zip.h"


static bool saveZipContent(content_t *content, struct zip_t *zip) {
    bool ok = true;
    char const *err;
    for (content_t *p = content; p; p = p->next) {
        switch (p->type) {
        case Skipped: case Missing:
            break;
        case Library:
            ok = saveZipContent(p->lbrHead, zip) && ok;
            break;
        }
        if (p->type == Skipped || p->type == Library || p->type == Missing)
            continue;
        // zip names should have / directory separator
        // also savePaths have an initial directory so this should be ignored
#ifdef _WIN32
        for (char *s = strchr(p->savePath, '\\'); s; s = strchr(s, '\\'))
            *s = '/';
#endif
        char const *zpath = strchr(p->savePath, '/');
        if (zpath)
            zpath++;
        else
            zpath = p->savePath;

        err = "";
        if (zip_entry_open(zip, zpath) != 0) {
            err = " - failed to open";
            ok = false;
        } else if (zip_entry_write(zip, p->out.buf, (size_t) p->out.pos) != 0) {
            err = " - failed to write";
            zip_entry_close(zip, p->out.fdate);
            ok = false;
        } else if (zip_entry_close(zip, p->out.fdate) != 0) {
            err = " - failed to close";
            ok = false;
        }
        if (nameCmp(nameOnly(zpath), p->out.fname) != 0)
            printf("%s -> %s%s\n", p->out.fname, zpath, err);
        else if (*err)
            printf("%s%s\n", zpath, err);
    }
    return ok;
}

bool saveZip(content_t *content, char const *targetDir, char const *zipfile) {
    bool ok = true;
    char const *zipPath = concat(targetDir, OSDIRSEP, zipfile, NULL);

    struct zip_t *zip = zip_open(zipPath, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
    if (zip == NULL) {
        printf("%s - cannot create zip file\n", zipPath);
        return false;
    }
    ok = saveZipContent(content, zip);
    zip_close(zip);
    
    setFileTime(zipPath, content->in.fdate);
    
    if (!ok) {
        printf("%s - problems processing file, deleting\n", zipPath);
        unlink(zipPath);
    }
    return ok;
}
