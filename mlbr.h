// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// This is an independent project of an individual developer. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// Copyright (c) 2020 Mark Ogden 
#ifndef MLBR_H
#define MLBR_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>


#ifdef _MSC_VER
#include <sys/utime.h>
#include <direct.h>
#define ISDIRSEP(c) ((c) == '/' || (c) == '\\')
#define DIRSEP  "/\\"
#define mkdir(path, mode)   _mkdir(path)
#define fileno  _fileno
#define futime  _futime
#define strdup  _strdup
#define unlink  _unlink
#define nameCmp _stricmp
#define strlwr  _strlwr
#else
#include <unistd.h>
#include <utime.h>
#include <limits.h>     // for PATH_MAX
#define ISDIRSEP(c) ((c) == '/')
#define DIRSEP  "/"
char *strlwr(char *str);
#define _MAX_PATH   PATH_MAX
#define nameCmp strcmp
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
    BADCRC = 1, NOCRC = 2, TRUNCATED = 4, ILLEGALCHAR = 8,       // bit flags
    RESERVED = 16, DEFDATE = 32, INCONTAINER = 128
};

#ifdef TESTING
#define MAX_HEADER  40
#else
#define MAX_HEADER  256
#endif



#define MINALLOC   1024
typedef struct {
    long bufSize;
    long pos;
    time_t fdate;
    char *fname;
    uint8_t *buf;
} file_t;

typedef struct _content {
    struct _content *next;
    uint8_t type;
    uint8_t status;
    uint8_t bitCount;
    unsigned bitStream;
    int length;             // this is the expected input length, in.bufSize is actual length
    char *saveName;
    char *comment;
    file_t in;
    file_t out;
} content_t;

enum {
    stored = 1, squeezed, crunched, crunchV1, crunchV2,
    crLzh, crLzhV1, crLzhV2, library, skipped, missing, mapping
};


enum {
    List = 1, Disk, Zip7
};
uint16_t crc16(uint8_t const *data, long len);

uint16_t crc(uint8_t const *data, long len);

time_t getFileTime(FILE *fp);
void setFileTime(char const *path, time_t ftime);





time_t cpmToOsTime(unsigned lbrDay, unsigned lbrTime);
time_t getCrunchTime(uint8_t const *pTime);

void list(content_t *content);
int chkName(char const *fn);

void mkOsNames(content_t *content);
bool parseLbr(content_t *content);


void *xmalloc(size_t size);
void *xcalloc(size_t count, size_t size);
void *xrealloc(void *p, size_t size);
char *xstrdup(char const *s);
file_t *loadFile(char const *name);
content_t *makeDescriptor(file_t const *file, char const *name, uint8_t *start, long length);
bool saveContent(content_t const *content, char const *dir);
void freeAllDescriptors(content_t *content);
void outU8(uint8_t c, content_t *content);
void outStr(content_t *content, char const *s);
void outRle(int val, content_t *content);
bool isEof(content_t const *content);
int inU8(content_t *content);
int inU16(content_t *content);
int inI16(content_t *content);
int u16At(uint8_t const *buf, long offset);
bool inSeek(content_t *content, long offset);
int inBits(content_t *content, uint8_t count);
int inBitRev(content_t *content);
void *xcalloc(size_t count, size_t size);
void unloadFile(file_t *file);
void setStoreFile(content_t *content);

bool unsqueeze(content_t *content);
bool uncrunch(content_t *content);
bool uncrLzh(content_t *content);
bool parseHeader(content_t *content);
bool rmkdir(char const *dir);
void usage(char const *fmt, ...);
char *mapCase(char *s);
bool saveZipContent(content_t const *content, char const *dir, char const *name);
char *replaceExt(char const *name, char const *ext);
#endif
