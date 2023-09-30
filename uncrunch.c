/* mlbr - extract .lbr archives and decompress Squeeze, Crunch (v1 & v2)
 *        and Cr-Lzh(v1 & v2) files.
 *	Comments and date stamps are supported as is conversion to .zip file
 *	Copyright (C) - 2020-2023 Mark Ogden
 *
 * uncrunch.c - 
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


/*-----------Original Copyright information-----------------------------------*/
/*  UNCRunch/C - LZW uncruncher compatible with Z-80 CP/M program CRUNCH 2.3  */
/*			(C) Copyright 1986 - Frank Prindle	                              */
/*		    May be reproduced for non-profit use only		                  */
/*----------------------------------------------------------------------------*/
/* This program is largely based on the previous works of Steven Greenberg,   */
/* Joe Orost, James Woods, Ken Turkowski, Steve Davies, Jim McKie, Spencer    */
/* Thomas, Terry Welch, and (of course) Lempel and Zev (Ziv?), whoever they   */
/* are.									                                      */
/* The original V1 hash algorithm Copyright (C) 2001 Russell Marks            */
/* as used in his lbrate-1.1 program                                          */
/*----------------------------------------------------------------------------*/
/*
    Modified by Mark Ogden (Feb 2020) to work with mlbr and refector code

*/
#include "mlbr.h"
#include <limits.h>

/*Macro definition - ensure letter is lower case*/
#define MAXSTR           4096

#define TABLE_SIZE       4096 /*size of main lzw table for 12 bit codes*/
#define XLATBL_SIZE      5003 /*size of physical translation table*/

/*special values for predecessor in table*/
#define NOPRED           0x6fff /*no predecessor in table*/
#define EMPTY            0x8000 /*empty table entry (xlatbl only)*/
#define REFERENCED       0x2000 /*table entry referenced if this bit set*/
#define IMPRED           0x7fff /*impossible predecessor*/

#define EOFCOD           0x100 /*special code for end-of-file*/
#define RSTCOD           0x101 /*special code for adaptive reset*/
#define NULCOD           0x102 /*special filler code*/
#define SPRCOD           0x103 /*spare special code*/
#define RESERVEDCODES    4

#define REPEAT_CHARACTER 0x90 /*following byte is repeat count*/

typedef struct {
    uint16_t predecessor; /*index to previous entry, if any*/
    uint16_t suffix;      /*character suffixed to previous entries*/
} entry_t;

static entry_t table[TABLE_SIZE];

/*auxilliary physical translation table*/
/*translates hash to main table index*/
static uint16_t xlatbl[XLATBL_SIZE];

/*other global variables*/
static uint8_t codlen;  /*variable code length in bits (9-12)*/
static uint8_t fulflg;  /*full flag - set once main table is full*/
static uint16_t entry;  /*next available main table entry*/
static bool entflg;     /*inhibit main loop from entering this code*/
static int finchar;     /*first character of last substring output*/
static uint16_t lastpr; // previous predecessor
static bool corrupt = false;
static bool isV2;   // true if V2 of Crunch
static int endcode; // code to mark end of input stream

/*
    Crunch time is stored in 3 fields
    createTime  0-4
    accessTime  5-9
    modifyTime  10-14

    each field is encoded in bcd
    year     - assume < 78 is 2000 onwards
    month
    day
    hour
    minute
    0xff -> 0
*/
static int bcd2Int(uint8_t n) {
    if (n == 0xff) {
        return 0;
    }
    if (n > 0x99 || n % 16 > 9) {
        return -1;
    }
    return (n / 16) * 10 + n % 16;
}

time_t getCrunchTime(uint8_t const *dateStamp) {
    struct tm timebuf;
    static struct {
        uint8_t low, high;
    } check[] = { { 0, 99 }, { 1, 12 }, { 1, 31 }, { 0, 23 }, { 0, 59 } };
    int16_t dateValue[5];

    if (bcd2Int(dateStamp[11]) > 0) { // is modify specified
        dateStamp += 10;              // is so use it as the base
    }

    for (int i = 0; i < 5; i++) {
        if ((dateValue[i] = bcd2Int(dateStamp[i])) < check[i].low || dateValue[i] > check[i].high) {
            return 0;
        }
    }
    timebuf.tm_year  = dateValue[0] + (dateValue[0] < 78 ? 100 : 0);
    timebuf.tm_mon   = dateValue[1] - 1;
    timebuf.tm_mday  = dateValue[2];
    timebuf.tm_hour  = dateValue[3];
    timebuf.tm_min   = dateValue[4];
    timebuf.tm_sec   = 0;
    timebuf.tm_isdst = -1;
    return _mkgmtime(&timebuf);
}

/*hash pred/suff into xlatbl pointer*/
/*duplicates the hash algorithm used by CRUNCH 2.3*/
uint16_t hashV2(uint16_t pred, uint16_t suff) {
    // returns hash value 1-4096
    if (suff == IMPRED) {
        suff = 0;
    }
    return ((((pred >> 4) & 0xff) ^ suff) | ((pred & 0xf) << 8)) + 1;
}

// hash function for V1
// generatel initial hash, then get new hash value from xlatbl if already inuse
static uint16_t hashV1(uint16_t pred, uint16_t chr) {
    uint16_t hashval;
    if (pred == IMPRED && chr == 0) {
        hashval = 0x800; /* special case (leaving the zero code free for EOF) */
    } else {
        uint16_t a = (((pred + chr) | 0x800) & 0x1fff);
#if UINT_MAX > 0x40000000
        /* if intermediate calculations are > 8192 * 8192 then there is no need to
         * prescale the numbers for multiply
         */ 
        hashval = (a * a >> 6) & 0xfff;
#else   /* original V1 algorithm from lbrate-1.1 with scaling for intermediate calculations */
        uint16_t b = a >> 1;
        hashval    = (((b * (b + (a & 1))) >> 4) & 0xfff);
#endif
    }

    // use link chain to find free slot
    while (table[hashval].suffix != EMPTY && xlatbl[hashval] != EMPTY) {
        hashval = xlatbl[hashval];
    }
    return hashval;
}

static uint16_t getInsertPtV1(uint16_t pred, uint8_t chr) {
    uint16_t hashval = hashV1(pred, chr);

    /* make sure we return early if possible to avoid adding link */
    if (table[hashval].suffix != EMPTY) {
        // probe for an empty slot starting 101 slots from initial slot
        uint16_t initialHash = hashval;

        for (hashval = (hashval + 101) % TABLE_SIZE; table[hashval].suffix != EMPTY;
             hashval = (hashval + 1) % TABLE_SIZE) {
            ;
        }
        // add link to here from the end of the chain
        xlatbl[initialHash] = hashval;
    }
    return hashval;
}

// find an empty entry in xlatbl which hashes from this predecessor/suffix
// combo, and store the index of the next available lzw table entry in it
// returns entry is always the the insert point into table
static uint16_t getInsertPtV2(uint16_t pred, uint8_t suff) {
    uint16_t hashval = hashV2(pred, suff);
    uint16_t rehash;

    /*follow secondary hash chain as necessary to find an empty slot*/
    for (rehash = hashval; xlatbl[rehash] != EMPTY; rehash = (rehash + hashval) % XLATBL_SIZE) {
        ;
    }

    /*stuff next available index into this slot*/
    xlatbl[rehash] = entry;
    return entry;
}

/*enter the next code into the lzw table*/
/* in ver 1.x crunched files, we hash the code into the array. This-
 * means we don't have a real entry, but entry is used to count
 * how many hashs have been created
 */
static void enterx(uint16_t pred, uint8_t suff) {
    uint16_t insertPt = isV2 ? getInsertPtV2(pred, suff) : getInsertPtV1(pred, suff);

    /*make the new entry*/
    table[insertPt].suffix = suff;
    if (isV2 || pred < MAXSTR) {
        table[insertPt].predecessor = pred;
    }

    /*if only one entry of the current code length remains, update to*/
    /*next code length because main loop is reading one code ahead*/
    if (++entry >= ~(~0U << codlen)) {
        if (codlen < 12) { // table not full, just make length one more bit
            codlen++;
        } else {      // table almost full (fulflg==0) or full (fulflg==1)
            fulflg++; // just increment fulflg - when it gets to 2  will never call again
        }
    }
}

/*initialize the lzw and physical translation tables and key decoder parameters */
static void initDecoder() {

    codlen  = isV2 ? 9 : 12;     // initial code length V1 is always 12
    fulflg  = 0;                 // flag as empty table
    entry   = isV2 ? 0 : 1;      // V1 pre allocated entry 0
    entflg  = true;              // first code is always atomic
    endcode = isV2 ? EOFCOD : 0; // end of date code

    /*first mark all entries of xlatbl as empty*/
    for (int i = 0; i < XLATBL_SIZE; i++) { // v1 only really needs MAXSTR
        xlatbl[i] = EMPTY;
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        table[i].suffix = table[i].predecessor = EMPTY; // v2 only really needs suffix
    }

    if (!isV2) {
        table[0].predecessor = table[0].suffix = IMPRED; /* reserved */
    }

    /*enter the 256 atomic into lzw table*/
    for (int i = 0; i < 0x100; i++) {
        enterx(isV2 ? NOPRED : IMPRED, i);
    }
    if (isV2) { // enter the 4 reserve codes
        for (int i = 0; i < RESERVEDCODES; i++) {
            enterx(IMPRED, 0); /*reserved codes*/
        }
    }
}

// get the next codlen bits from the input stream
// for V2 skip the filler codes
// errors or end codes are seen then return EOF instead
static int getcode(content_t *content) {
    int code;
    do {
        code = inBits(content, codlen);
    } while (isV2 && (code == NULCOD || code == SPRCOD));
    return code == endcode ? EOF : code;
}

// emit the byte string for this code
static bool decode(uint16_t code, content_t *content) {
    if (table[code].suffix == EMPTY) {
        // we need to insert this code before using it
        entflg = true; // prevent main loop inserting again
        enterx(lastpr, finchar);
    }
    if (isV2) {
        table[code].predecessor |= REFERENCED;
    }

    uint8_t stack[MAXSTR];
    uint8_t *stackp = stack;
    // pick up the byte string from the tables which are stored in reverse order
    // V1 uses empty predecessor to note last
    // V2 uses code in range 0-255
    while ((!isV2 && table[code].predecessor != EMPTY) || (isV2 && code > 255)) { //-V781
        *stackp++ = (uint8_t)table[code].suffix;
        code      = table[code].predecessor % TABLE_SIZE;
        if (stackp >= &stack[MAXSTR]) {
            corrupt = true;
            return (entflg);
        }
    }

    // send the first byte and record if for later processing
    outRle(finchar = table[code].suffix, content);

    // emit the rest of the byte string
    while (stackp > stack) { /*the rest*/
        outRle(*--stackp, content);
    }

    return (entflg);
}

// attempt to reassign an existing code which has been defined, but never referenced
static void entfil(uint16_t pred, uint8_t suff) {
    uint16_t hashval = hashV2(pred, suff);

    /*search the candidate codes (all those which hash from this new*/
    /*predecessor and suffix) for an unreferenced one*/
    for (uint16_t curhash = hashval; xlatbl[curhash] != EMPTY;
         curhash          = (curhash + hashval) % XLATBL_SIZE) {
        /*candidate code*/
        entry_t *ep = table + xlatbl[curhash];
        if (!(ep->predecessor & REFERENCED)) { // entry reassignable, so do it!
            ep->predecessor = pred;
            ep->suffix      = suff;
            /*discontinue search*/
            break;
        }
    }
}

// this is the main loop to process the crunched data

static bool uncrunchData(content_t *content) {
    initDecoder();       // set up atomic code definitions etc
    outRle(-1, content); // reset rle engine
    corrupt = false;     // no corruption detected yet

    int pred;
    for (lastpr = NOPRED; !corrupt && (pred = getcode(content)) >= 0; lastpr = pred) {
        if (isV2 && pred == RSTCOD) { // reset code
            initDecoder();
            pred = NOPRED;
        } else if (fulflg != 2) { // a normal code room in table
            if (decode(pred, content) == false) {
                enterx(lastpr, finchar); // enter code if decode didn't already do so
            } else {
                entflg = false; // reset the toggle so next enterx works
            }
        } else { // table is full
            decode(pred, content);
            if (isV2) { // V2 attempts to reassign
                entfil(lastpr, finchar);
            }
        }
    }
    return !corrupt;
}

/*uncrunch a single file return true for successful uncrunch */
int uncrunch(content_t *content) {
    uint8_t reflevel;  /*ref rev level from input file*/
    uint8_t siglevel;  /*sig rev level from input file*/
    uint8_t errdetect; /*error detection flag from input file*/

    if (!parseHeader(content)) { // shared header processing
        return BADHEADER;
    }

    /*read the four info bytes*/
    reflevel  = inU8(content);
    siglevel  = inU8(content);
    errdetect = inU8(content);
    if (inU8(content) < 0) { /*skip spare but check for eof*/
        return BADHEADER;
    }

    // check uncrunch version is supported
    if (siglevel < 0x10 || siglevel > 0x2f) {
        printf("%s unsupported version of crunch\n", content->in.fname);
        return BADHEADER;
    }

    isV2          = siglevel >= 0x20; // file global that reflects version of crunch

    content->type = isV2 ? CrunchV2 : CrunchV1; // update the type to reflect we know the version

    if (!uncrunchData(content)) { // go do the decode
        return CORRUPT;
    }

    /*verify checksum if required*/
    int fileCrc = inU16(content);
    if (fileCrc < 0) {
        return CORRUPT;
    }
    // tests below will return BADCRC or GOOD
    if (errdetect == 1) {
        return crc16(content->out.buf, content->out.pos) == fileCrc;
    }
    if (errdetect == 0) {
        return crc(content->out.buf, content->out.pos) == fileCrc;
    }
    return GOOD;
}
