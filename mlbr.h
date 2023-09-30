// Copyright (c) 2020-2022 Mark Ogden 
#ifndef MLBR_H
#define MLBR_H

// make sure MSVC doesn't moan about names
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>



#ifdef _MSC_VER
#include <sys/utime.h>
#include <direct.h>
#define ISDIRSEP(c) ((c) == '/' || (c) == '\\')
#define DIRSEP  "/\\"
#define OSDIRSEP   "\\"
#define NEWLINE "\r\n"
#define mkdir(path, mode)   _mkdir(path)
#define timegm _mkgmtime

#define nameCmp _stricmp
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#define alloca  _alloca
#define realpath(path, resolved)    _fullpath(resolved, path, 0)
#else
#include <unistd.h>
#include <utime.h>
#include <limits.h>     // for PATH_MAX
#define ISDIRSEP(c) ((c) == '/')
#define DIRSEP  "/"
#define OSDIRSEP "/"
#define NEWLINE "\n"
char *strlwr(char *str);
#define _MAX_PATH   PATH_MAX
#define nameCmp strcmp
int _vscprintf(const char *fmt, va_list pargs);
#endif


#define CPMEOF  0x1a
#define REPEAT_CHAR 0x90


// LBR directory offsets
enum {
    Status = 0, Name = 1, Ext = 9, Index = 12,
    Length = 14, Crc = 16, CreateDate = 18, ChangeDate = 20,
    CreateTime = 22, ChangeTime = 24, PadCnt = 26, Filler = 27
};

#define LBRDIR_SIZE 32
#define LBRSECTOR_SIZE  128
enum {
    F_BADCRC = 1, F_NOCRC = 2, F_TRUNCATED = 4       // bit flags
};

enum {
    LISTONLY = 0, EXTRACT = 1, SUBDIR = 2, ZIP = 4, Z7 = 8, SAVEMASK = 0xf,
    FORCE = 16, RECURSE = 32, KEEPCASE = 64, NOEXPAND = 128
};

enum {      // return results from decompression functions
    BADHEADER = -2, CORRUPT = -1 , BADCRC = 0, GOOD = 1
};
#ifdef TESTING
#define MAX_HEADER  40
#else
#define MAX_HEADER  256
#endif

extern bool ignoreCrc;
extern bool ignoreCorrupt;

#define MINALLOC   1024
typedef struct {
    long bufSize;
    long pos;
    time_t fdate;
    char const *fname;
    uint8_t *buf;
} file_t;

typedef struct _content {
    struct _content *next;
    struct _content *lbrHead;
    uint16_t status;
    uint8_t type;
    uint8_t bitCount;
    unsigned bitStream;
    int length;             // this is the expected input length, in.bufSize is actual length
    char const *savePath;   // name file is saved as including directory prefix
    char *msg;
    char const *comment;
    file_t in;
    file_t out;
} content_t;

enum {
    Stored = 1, Squeezed, Crunched, CrunchV1, CrunchV2,
    CrLzh, CrLzhV1, CrLzhV2, Library, Skipped, Missing, Mapping
};

uint16_t crc16(uint8_t const *data, long len);

uint16_t crc(uint8_t const *data, long len);

time_t getFileTime(FILE *fp);
void setFileTime(char const *path, time_t ftime);





time_t cpmToOsTime(unsigned cpmDay, unsigned timeInSecs);
time_t getCrunchTime(uint8_t const *dateStamp);

void list(content_t *content, time_t defDate, int depth);

void mkOsNames(content_t *content, char const *targetDir, int flags);
bool parseLbr(content_t *content);

char const *methodName(content_t *content);

void *xmalloc(size_t size);
void *xcalloc(size_t count, size_t size);
void *xrealloc(void *p, size_t size);
char *xstrdup(char const *s);
char *sAlloc(size_t n);
void sFree();
void dumpNames();
bool safeMkdir(char const *dir);

const char *uniqueName(char const *subDir, char const *fname);

file_t *loadFile(char const *name);
content_t *makeDescriptor(file_t const *file, char const *name, uint8_t *start, long length);
bool saveContent(content_t const *content, char const *targetDir);
void freeAllDescriptors(content_t *content);
void outU8(uint8_t c, content_t *content);
void outStr(content_t *content, char const *fmt, ...);
void outRle(int val, content_t *content);
bool isEof(content_t const *content);
int inU8(content_t *content);
int inU16(content_t *content);
int inI16(content_t *content);
int u16At(uint8_t const *buf, long offset);
bool inSeek(content_t *content, long offset);
int inBits(content_t *content, uint8_t count);
int inBitRev(content_t *content);
void unloadFile(file_t *file);
void setStoreFile(content_t *content);

int unsqueeze(content_t *content);
int uncrunch(content_t *content);
int uncrLzh(content_t *content);
bool parseHeader(content_t *content);
bool mkPath(char const *dir);
void usage(char const *fmt, ...);
char *mapCase(char *s);
bool saveZip(content_t *content, char const *targetDir, char const *zipfile);
char *replaceExt(char const *name, char const *ext);
char const *nameOnly(char const *fname);
void protectSrc(const char *fname, const char *target);
void freeHashTable();
bool chkClash(char const *fname);
void displayDate(time_t date);
void logErr(content_t *content, char const *fmt, ...);
char const *concat(const char *s, ...);
char const *makeFullPath(const char *targetDir, const char *fname);
#ifdef _DEBUG
void xfree(void *p);
#else
#define xfree(p)    free(p)
#endif
#endif
