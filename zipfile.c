/* mlbr - extract .lbr archives and decompress Squeeze, Crunch (v1 & v2)
 *        and Cr-Lzh(v1 & v2) files.
 *	Comments and date stamps are supported as is conversion to .zip file
 *	Copyright (C) - 2020-2023 Mark Ogden
 *
 * zipfile.c - wrappers to safe zip content to files
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
#define MINIZ_HEADER_FILE_ONLY
#include "miniz.h"
#include "zip.h"

static bool saveZipContent(content_t *content, struct zip_t *zip) {
    bool ok = true;
    char const *err;
    for (content_t *p = content; p; p = p->next) {
        switch (p->type) {
        case Skipped:
        case Missing:
            break;
        case Library:
            ok = saveZipContent(p->lbrHead, zip) && ok;
            break;
        }
        if (p->type == Skipped || p->type == Library || p->type == Missing) {
            continue;
        }
        // zip names should have / directory separator
        // also savePaths have an initial directory so this should be ignored
#ifdef _WIN32
        for (char *s = strchr(p->savePath, '\\'); s; s = strchr(s, '\\')) {
            *s = '/';
        }
#endif
        char const *zpath = strchr(p->savePath, '/');
        if (zpath) {
            zpath++;
        } else {
            zpath = p->savePath;
        }

        err = "";
        if (zip_entry_open(zip, zpath) != 0) {
            err = " - failed to open";
            ok  = false;
        } else if (zip_entry_write(zip, p->out.buf, (size_t)p->out.pos) != 0) {
            err = " - failed to write";
            zip_entry_close(zip, p->out.fdate);
            ok = false;
        } else if (zip_entry_close(zip, p->out.fdate) != 0) {
            err = " - failed to close";
            ok  = false;
        }
        if (nameCmp(nameOnly(zpath), p->out.fname) != 0) {
            printf("%s -> %s%s\n", p->out.fname, zpath, err);
        } else if (*err) {
            printf("%s%s\n", zpath, err);
        }
    }
    return ok;
}

bool saveZip(content_t *content, char const *targetDir, char const *zipfile) {
    bool ok             = true;
    char const *zipPath = concat(targetDir, OSDIRSEP, zipfile, NULL);

    struct zip_t *zip   = zip_open(zipPath, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
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
