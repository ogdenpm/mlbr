#include "mlbr.h"
#define MINIZ_HEADER_FILE_ONLY
#include "miniz.h"
#include "zip.h"


bool saveZipContent(content_t const *content, char const *dir, char const *name) {
    bool ok = true;
    struct zip_t *zip = NULL;
    
    char *path = xmalloc(strlen(dir) + strlen(name) + 2);
    strcpy(path, dir);
    char *s = strchr(path, '\0');
    if (s != path && !ISDIRSEP(s[-1]))
        *s++ = '/';
    strcpy(s, name);

    for (content_t const *p = content; p; p = p->next) {
        if (p->type == skipped || p->type == library || p->type == missing)
            continue;
        if (zip == NULL)
            zip = zip_open(path, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');

        if (zip == NULL)
            break;

        printf("%-12s", p->out.fname);

        if (strcmp(path, p->out.fname) != 0)
            printf(" ->%s %s", p->out.fname == p->saveName ? "" : "*", p->saveName);
        putchar('\n');
        zip_entry_open(zip, p->saveName);
        zip_entry_write(zip, p->out.buf, p->out.pos);
        zip_entry_close(zip, p->out.fdate);
    }

    if (zip) {
        zip_close(zip);
        setFileTime(path, content->in.fdate);
    } else {
        printf("could not create %s\n", path);
        ok = false;
    }
    free(path);
    putchar('\n');
    return ok;
}