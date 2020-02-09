// Copyright (c) 2020 Mark Ogden 
#include "mlbr.h"

char const *nameOnly(char const *fname) {
    char const *s;
    
    if (s = strrchr(fname, '/'))                   // find file name assuming / directory separator
        fname = s + 1;

#ifdef _WIN32

    if (s = strrchr(fname, '\\'))                   // find file name assuming \ directory separator
        fname = s + 1;
    if (s = strchr(fname, ':'))                     // also skip any x:
        fname = s + 1;
#endif
    return fname;

}



file_t *loadFile(char const *name) {
    FILE *fp;
    struct stat statBuf;

    if ((fp = fopen(name, "rb")) == NULL || fstat(fileno(fp), &statBuf) != 0) {
        if (fp)
            fclose(fp);
        usage("Cannot open %s\n", name);
    }

    file_t *file = xcalloc(1, sizeof(file_t));
    file->bufSize = statBuf.st_size;
    file->buf = xmalloc(file->bufSize);
    if (fread(file->buf, 1, file->bufSize, fp) != file->bufSize) {
        fclose(fp);
        free(file->buf);
        usage("Problem reading %s\n", name);
    }
    file->fdate = getFileTime(fp);
    file->fname = mapCase(xstrdup(nameOnly(name)));
    fclose(fp);
    return file;
}

void unloadFile(file_t *file) {
    free(file->buf);
    free(file);
}


content_t *makeDescriptor(file_t const *file, char const *name, uint8_t *start, long length) {
    content_t *content = xcalloc(1, sizeof(content_t));
    content->in.buf = start;
    content->in.fname = mapCase(xstrdup(name));
    content->in.fdate = file->fdate;
    content->length = length;           // expected length

 
    if (start && start + length > file->buf + file->bufSize) {     // is it past the end of the file
        content->in.bufSize = (long)((file->buf + file->bufSize) - start);
        if (content->in.bufSize < 0)
            printf("!! %s missing all data\n", name);
        else
            printf("!! %s missing %d bytes of data\n", name, length - content->in.bufSize);
        content->status |= TRUNCATED;
    } else
        content->in.bufSize = length;

    return content;
}


bool saveContent(content_t const *content, char const *dir) {
    bool ok = true;
    bool dirMsg = false;
    for (; content; content = content->next) {
        if (content->type == skipped || content->type == library || content->type == missing)
            continue;

        char *path;

        if (dir && *dir != 0) {
            path = xmalloc(strlen(dir) + strlen(content->saveName) + 2);
            strcpy(path, dir);
            char *s = strchr(path, '\0');
            if (!ISDIRSEP(s[-1]))
                strcat(s, "/");
            strcat(s, content->saveName);
        } else {
            path = xmalloc(strlen(content->saveName) + 1);
            strcpy(path, content->saveName);
        }
        printf("%-12s", content->out.fname);

        if (strcmp(path, content->out.fname) != 0)
            printf(" ->%s %s", content->out.fname == content->saveName ? "" : "*", path);

        FILE *fp = fopen(path, "wb");
        if (fp == NULL) {
            printf(" - could not create file\n");
            ok = false;
        } else if (fwrite(content->out.buf, 1, content->out.pos, fp) != content->out.pos) {
            fclose(fp);
            unlink(path);
            printf(" - problem writing file\n");
            ok = false;
        } else {
            fclose(fp);
            setFileTime(path, content->out.fdate);
            printf(" - ok\n");
        }
    }
    putchar('\n');
    return ok;
}

static void freeDescriptor(content_t *content) {
    if (content->comment)
        free(content->comment);
    if (content->saveName != content->out.fname)    // only free if not specially generated
        free(content->saveName);
    if (content->out.fname != content->in.fname)    // only free if not stored entry
        free(content->out.fname);
    free(content->in.fname);
    if (content->out.buf && content->out.buf != content->in.buf)        // output may have been explicitly allocated
        free(content->out.buf);
    free(content);
}

void freeAllDescriptors(content_t *content) {
    content_t *p, *q;

    for (p = content; p; p = q) {         // free all of the sub content info
        q = p->next;
        freeDescriptor(p);
    }
}


void outU8(uint8_t c, content_t *content) {
    if (content->out.pos >= content->out.bufSize) {
        if (content->out.bufSize)
            content->out.bufSize *= 2;
        else
            content->out.bufSize = content->in.bufSize * 2 < MINALLOC ? MINALLOC : content->in.bufSize * 2;
        content->out.buf = xrealloc(content->out.buf, content->out.bufSize);
    }
    content->out.buf[content->out.pos++] = c;
}

void outStr(content_t *content, char const *s) {
    while (*s)
        outU8(*s++, content);
}

void outRle(int val, content_t *content) {
    static bool repeatFlag;
    static int lastCh;

    if (val < 0)
        repeatFlag = false;
    else if (repeatFlag) {
        repeatFlag = false;
        if (val == 0)
            outU8(REPEAT_CHAR, content);
        else
            while (--val > 0)
                outU8(lastCh, content);
    } else if (val == REPEAT_CHAR)
        repeatFlag = true;
    else
        outU8(lastCh = val, content);
}



bool isEof(content_t const *content) {
    return content->in.pos >= content->in.bufSize;
}

int inU8(content_t  *content) {
    return isEof(content) ? EOF : content->in.buf[content->in.pos++];
}

int inU16(content_t *content) {
    int low = inU8(content);
    return isEof(content) ? EOF : low + inU8(content) * 256;
}

int inI16(content_t *content) {
    int low = inU8(content);
    int high = inU8(content);
    return high < 0 ? EOF : (int16_t)(low + high * 256);
}

bool inSeek(content_t *content, long offset) {
    if (offset >= content->in.bufSize)
        return false;
    content->in.pos = offset;
    return true;
}

int inBits(content_t *content, uint8_t count) {
    while (count > content->bitCount) {
        if (isEof(content))
            return EOF;
        content->bitStream = (content->bitStream << 8) + content->in.buf[content->in.pos++];
        content->bitCount += 8;
    }
    content->bitCount -= count;
    return (content->bitStream >> content->bitCount) & ~(~0U << count);

}

int inBitRev(content_t *content) {
    if ((content->bitStream >>= 1) <= 1) {  // no data left
        if (isEof(content))
            return EOF;
        content->bitStream = content->in.buf[content->in.pos++] + 0x100;    // byte + marker
    }
    return content->bitStream & 1;
}

void setStoreFile(content_t *content) {
    if (content->out.buf)                                                   // free any buffer allocated
        free(content->out.buf);
    if (content->out.fname)                                                 // free any name allocated                                                 
        free(content->out.fname);
    if (content->comment) {
        free(content->comment);
        content->comment = NULL;
    }
    content->out = content->in;                                             // set up to store / skip the file
    // set pos to real expected input length (skipped and missing) will not save using this
    content->out.pos = content->length;

}