/* mlbr - extract .lbr archives and decompress Squeeze, Crunch (v1 & v2)
 *        and Cr-Lzh(v1 & v2) files.
 *	Comments and date stamps are supported as is conversion to .zip file
 *	Copyright (C) - 2020-2023 Mark Ogden
 *
 * memory.c - safe memory allocation
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

/*
    To avoid the need to track many string allocations and free them
    the code here implements a simple string allocation process
    If there are less than STRALLOC bytes used then no dynamic memory is
    used else additional STRALLOC blocks are allocated as necessary
    for requests > STRALLOC then the requested size + STRALLOC is allocated
    sFree is used to free any dynamic strings
*/
#define STRALLOC 8192

int allocCnt;

typedef struct _str {
    struct _str *next;
    size_t lastLoc;
    size_t strSize;
    char str[STRALLOC];
} str_t;

static str_t stringMem = { .strSize = STRALLOC };

char *sAlloc(size_t n) {
    for (str_t *p = &stringMem;; p = p->next) {
        if (p->lastLoc + n <= p->strSize) {
            char *str = p->str + p->lastLoc;
            p->lastLoc += n;
            return str;
        }
        if (!p->next) {
            p->next          = xcalloc(1, sizeof(str_t) +
                                              (n > STRALLOC ? n : 0)); // for long string allocate big buffer
            p->next->strSize = STRALLOC + (n > STRALLOC ? n : 0);
        }
    }
}

void sFree() {
    str_t *q;
    for (str_t *p = stringMem.next; p; p = q) {
        q = p->next;
        xfree(p);
    }
    stringMem.lastLoc = 0;
    stringMem.next    = NULL;
}

// simple wrappers to standard malloc, calloc, realloc calls to check for out of memory
void *xmalloc(size_t size) {
#ifdef _DEBUG
    allocCnt++;
#endif
    void *p = malloc(size);
    if (p) {
        return p;
    }
    fprintf(stderr, "Fatal Error: Out of memory\n");
    exit(1);
}

void *xcalloc(size_t count, size_t size) {
#ifdef _DEBUG
    allocCnt++;
#endif
    void *p = calloc(count, size);
    if (p) {
        return p;
    }
    fprintf(stderr, "Fatal Error: Out of memory\n");
    exit(1);
}

void *xrealloc(void *p, size_t size) {
#ifdef _DEBUG
    if (p == NULL)
        allocCnt++;
#endif
    if ((p = realloc(p, size)) == NULL) {
        fprintf(stderr, "Fatal Error: Out of memory\n");
        exit(1);
    }
    return p;
}

#ifdef _DEBUG
void xfree(void *p) {
    if (p) {
        free(p);
        allocCnt--;
    }
}
#endif

// version of strdup that uses sAlloc
char *xstrdup(char const *s) {
    char *dup = sAlloc(strlen(s) + 1);
    strcpy(dup, s);
    return dup;
}

// create a new sAlloced based string that replaces the extent in name with ext
char *replaceExt(char const *name, char const *ext) {
    const char *s = strrchr(name, '.'); // start of extent
    if (!s) {
        s = strchr(name, '\0'); // no extent so end of name
    }
    char *newName = sAlloc(s - name + strlen(ext) + 1);
    strncpy(newName, name, s - name);
    strcpy(newName + (s - name), ext);
    return newName;
}

// simple manager to check for name clashes
// the relative path name of each file is stored using a hash table
// as clashes are checked across all of the command line specified files
// which could be large
typedef struct _name {
    struct _name *next;
    char const *fname;
} name_t;

#define HASHSIZE 4096
static name_t *hashTable[HASHSIZE];

// frees all of the names currently allocated
void freeHashTable() {
    for (size_t i = 0; i < HASHSIZE; i++) {
        if (hashTable[i]) {
            name_t *p;
            name_t *q;
            for (p = hashTable[i]; p; p = q) {
                q = p->next;
                xfree((void *)(p->fname));
                xfree(p);
            }
            hashTable[i] = NULL;
        }
    }
}

/*
    check whether fname clashes with previous names
    returns true if it does
    else adds the name to the used names and returns false
*/
bool chkClash(char const *fname) {
    uint16_t hash = crc16(fname, (long)strlen(fname)) % HASHSIZE; // use crc 16 to hash name
    if (hashTable[hash]) {
        for (name_t *p = hashTable[hash]; p; p = p->next) {
            if (nameCmp(p->fname, fname) == 0) { // already exists?
                return true;
            }
        }
    }
    name_t *p       = xmalloc(sizeof(name_t)); // no clash so insert into hash table
    p->fname        = fname;
    p->next         = hashTable[hash];
    hashTable[hash] = p;
    return false; // no clash
}

#if _DEBUG
// for debugging show what names have been used
void dumpNames() {
    for (unsigned i = 0; i < HASHSIZE; i++)
        if (hashTable[i]) {
            printf("%-5d", i);
            for (name_t *p = hashTable[i]; p; p = p->next) {
                printf(" %s", p->fname);
            }
            putchar('\n');
        }
}
#endif
