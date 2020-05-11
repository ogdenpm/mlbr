// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Copyright (c) 2020 Mark Ogden
/*
    functions to support in memory copies of files
*/
#include "mlbr.h"
#include <stdarg.h>


// load a real file into memory
file_t *loadFile(char const *name) {
    FILE *fp;
    struct stat statBuf;
    file_t *file = NULL;

    if ((fp = fopen(name, "rb")) == NULL) {
        puts(" cannot open");
        return NULL;
    }
    if (fstat(fileno(fp), &statBuf) != 0)
        puts(" problems reading");
    else {
        file = xcalloc(1, sizeof(file_t));
        file->bufSize = statBuf.st_size;
        file->buf = xmalloc((size_t)file->bufSize);
        if (fread(file->buf, 1, (size_t)file->bufSize, fp) == file->bufSize) {
            file->fdate = getFileTime(fp);
            file->fname = mapCase(xstrdup(nameOnly(name)));
        } else {
            xfree(file->buf);
            xfree(file);
            file = NULL;
            puts(" problem reading");
        }
    }
    fclose(fp);
    return file;
}


// release the real file and internal memory it used
void unloadFile(file_t *file) {
    xfree(file->buf);
    xfree(file);
}

// create a descriptor for a subset of a real file
// the descriptor is used to manage input / output for a compressed
// file along with supporting information to help with the decompression etc.

// assumes name is in persistent memory e.g. allocated by sAlloc
content_t *makeDescriptor(file_t const *file, char const *name, uint8_t *start, long length) {
    content_t *content = xcalloc(1, sizeof(content_t));
    content->in.buf = start;
    content->in.fname = name;
    content->in.fdate = file->fdate;
    content->length = length;           // expected length

 
    if (start && start + length > file->buf + file->bufSize) {     // is it past the end of the file
        content->in.bufSize = (long)((file->buf + file->bufSize) - start);
        if (content->in.bufSize < 0)
            logErr(content, "!! %s missing all data\n", name);
        else
            logErr(content, "!! %s missing %d bytes of data\n", name, length - content->in.bufSize);
        content->status |= F_TRUNCATED;
    } else
        content->in.bufSize = length;

    return content;
}

// free all allocated descriptors (string space is freed separately)

void freeAllDescriptors(content_t *content) {
    content_t *p, *q;

    for (p = content; p; p = q) {               // free all of the sub content info
        q = p->next;
        if (p->lbrHead)
            freeAllDescriptors(p->lbrHead);     // free up containers
        if (p->out.buf && p->out.buf != p->in.buf)
            xfree(p->out.buf);
        xfree(p);
    }
}

// takes a descriptor pointing to a potential chain of other descriptors
// and saves the decompressed content to real files
// library containers call this function recursively
// returns true if everything is ok
bool saveContent(content_t const *content, char const *targetDir) {
    bool ok = true;

    for (; content; content = content->next) {
        char const *savePath = makeFullPath(targetDir, content->savePath ? content->savePath : "");
        char const *err;
        switch (content->type) {
        case Skipped: case Missing:
            break;
        case Library:
            if (content->savePath) {
                if (!safeMkdir(savePath)) {
                    printf("%s - cannot create sub directory\n", content->savePath);
                    ok = false;
                } else
                    setFileTime(savePath, content->out.fdate);
            }
            ok = saveContent(content->lbrHead, targetDir) && ok;
            break;
        default:
            err = "";
            FILE *fp = fopen(savePath, "wb");
            if (fp == NULL) {
                err = " - could not create file";
                ok = false;
            } else if (fwrite(content->out.buf, 1, content->out.pos, fp) != content->out.pos) {
                fclose(fp);
                unlink(savePath);
                err = " - problem writing file";
                ok = false;
            } else {
                fclose(fp);
                setFileTime(savePath, content->out.fdate);
            }
            if (nameCmp(nameOnly(content->savePath), content->out.fname) != 0)
                printf("%s -> %s%s\n", content->out.fname, content->savePath, err);
            else if (*err)
                printf("%s%s\n", content->savePath, err);

        }
        xfree((void *)savePath);
    }
    return ok;
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



void outStr(content_t *content, char const *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    unsigned msgLen = _vscprintf(fmt, args) + 1;             // length of new message
    char *msg = alloca(msgLen);                             // use stack
    vsprintf(msg, fmt, args);
    va_end(args);
    while (*msg) {
        if (*msg == '\n')
            outU8('\r', content);
        outU8(*msg++, content);
    }
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
    int high = inU8(content); //-V656
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
    if ((content->bitStream >>= 1) <= 1) {  // no data left //-V1019
        if (isEof(content))
            return EOF;
        content->bitStream = content->in.buf[content->in.pos++] + 0x100;    // byte + marker
    }
    return content->bitStream & 1;
}

void setStoreFile(content_t *content) {
    xfree(content->out.buf);
    content->comment = NULL;
    content->out = content->in;                                             // set up to store / skip the file
    // set pos to real expected input length (skipped and missing) will not save using this
    content->out.pos = content->length;

}