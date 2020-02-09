/*----------------------------------------------------------------------------*/
/*  UNCRunch/C - LZW uncruncher compatible with Z-80 CP/M program CRUNCH 2.3  */
/*			(C) Copyright 1986 - Frank Prindle		      */
/*		    May be reproduced for non-profit use only		      */
/*----------------------------------------------------------------------------*/
/* This program is largely based on the previous works of Steven Greenberg,   */
/* Joe Orost, James Woods, Ken Turkowski, Steve Davies, Jim McKie, Spencer    */
/* Thomas, Terry Welch, and (of course) Lempel and Zev (Ziv?), whoever they   */
/* are.									      */
/*----------------------------------------------------------------------------*/
/*
    Modified by Mark Ogden (Feb 2020) to work with mlbr and refector code

*/
#include "mlbr.h"

/*Macro definition - ensure letter is lower case*/
#define MAXSTR      4096

#define TABLE_SIZE  4096	/*size of main lzw table for 12 bit codes*/
#define XLATBL_SIZE 5003	/*size of physical translation table*/

/*special values for predecessor in table*/
#define NOPRED 0x6fff		/*no predecessor in table*/
#define EMPTY  0x8000		/*empty table entry (xlatbl only)*/
#define REFERENCED 0x2000	/*table entry referenced if this bit set*/
#define IMPRED 0x7fff		/*impossible predecessor*/

#define EOFCOD 0x100		/*special code for end-of-file*/
#define RSTCOD 0x101		/*special code for adaptive reset*/
#define NULCOD 0x102		/*special filler code*/
#define SPRCOD 0x103		/*spare special code*/

#define REPEAT_CHARACTER 0x90	/*following byte is repeat count*/

typedef struct {
    uint16_t predecessor;	/*index to previous entry, if any*/
    uint16_t suffix;	/*character suffixed to previous entries*/
} entry_t;

entry_t table[TABLE_SIZE];

/*auxilliary physical translation table*/
/*translates hash to main table index*/
uint16_t xlatbl[XLATBL_SIZE];



/*other global variables*/
unsigned char	codlen;		/*variable code length in bits (9-12)*/
unsigned char	fulflg;		/*full flag - set once main table is full*/
uint16_t	entry;			/*next available main table entry*/
bool	entflg; 	/*inhibit main loop from entering this code*/
int	finchar;		/*first character of last substring output*/
uint16_t lastpr;    // previous predecessor
bool corrupt = false;

/*hash pred/suff into xlatbl pointer*/
/*duplicates the hash algorithm used by CRUNCH 2.3*/
uint16_t hash(uint16_t pred, uint8_t suff) {
    // returns hash value 1-4096
    if (suff == IMPRED)
        suff = 0;
    return ((((pred >> 4) & 0xff) ^ suff) | ((pred & 0xf) << 8)) + 1;
}

/*find an empty entry in xlatbl which hashes from this predecessor/suffix*/
/*combo, and store the index of the next available lzw table entry in it*/
void figure(uint16_t pred, uint8_t suff) {
    uint16_t hashval = hash(pred, suff);
    uint16_t curhash;

    /*follow secondary hash chain as necessary to find an empty slot*/
    for (curhash = hashval; xlatbl[curhash] != EMPTY; curhash = (curhash + hashval) % XLATBL_SIZE)
        ;

    /*stuff next available index into this slot*/
    xlatbl[curhash] = entry;
}

void enterxV1(uint16_t pred, uint8_t chr) {
    uint16_t lasthash, hashval;

    /* in ver 1.x crunched files, we hash the code into the array. This
     * means we don't have a real entry, but entry is used to count
     * how many hashs have been created
     */
    if (entry >= MAXSTR)
        return;
    entry++;

    if (pred == 0xffff && chr == 0)
        hashval = 0x800;   /* special case (leaving the zero code free for EOF) */
    else {
        /* normally we do a slightly awkward mid-square thing */
        uint16_t a = (((pred + chr) | 0x800) & 0x1fff);
        uint16_t b = (a >> 1);
        hashval = (((b * (b + (a & 1))) >> 4) & 0xfff);
    }

    /* first, check link chain from there */
    while (table[hashval].suffix != EMPTY && xlatbl[hashval] != IMPRED)
        hashval = xlatbl[hashval];

    /* make sure we return early if possible to avoid adding link */
    if (table[hashval].suffix != EMPTY) {
        lasthash = hashval;

        /* slightly odd approach if it's not in that - first try skipping
         * 101 entries, then try them one-by-one. If should be impossible
         * for this to loop indefinitely, since we made sure with the entry
         * test at the start
         */
        hashval = (hashval + 101) % TABLE_SIZE;

        for (int f = 0; f < MAXSTR && table[hashval].suffix != EMPTY; f++, hashval = (hashval + 1) % TABLE_SIZE)
            ;

        /* add link to here from the end of the chain */
        //printf("xlatbl[%04X] was %04X now %04X\n", lasthash, xlatbl[lasthash], hashval);
        xlatbl[lasthash] = hashval;
    }
    //printf("Enter %04X %02X %04X\n", hashval, chr, pred);
    table[hashval].suffix = chr;
    if (pred < MAXSTR)
        table[hashval].predecessor = pred;
}

/*enter the next code into the lzw table*/
void enterxV2(uint16_t pred, uint8_t suff) {
    // pred table index of predecessor
    // suff suffix byte represented by this enty

    /*update xlatbl to point to this entry*/
    figure(pred, suff);

    /*make the new entry*/
    table[entry].predecessor = pred;
    table[entry++].suffix = suff;


    /*if only one entry of the current code length remains, update to*/
    /*next code length because main loop is reading one code ahead*/
    if (entry >= ~(~0U << codlen))
        if (codlen < 12) // table not full, just make length one more bit
            codlen++;
        else            // table almost full (fulflg==0) or full (fulflg==1)
            fulflg++;   // just increment fulflg - when it gets to 2  will never call again
}



/*initialize the lzw and physical translation tables*/
void initb2() {
    /*first mark all entries of xlatbl as empty*/
    for (int i = 0; i < XLATBL_SIZE; i++)
        xlatbl[i] = EMPTY;

    /*enter atomic and reserved codes into lzw table*/
    for (int i = 0; i < 0x100; i++)
        enterxV2(NOPRED, i);	/*first 256 atomic codes*/
    for (int i = 0; i < 4; i++)
        enterxV2(IMPRED, 0);	/*reserved codes*/
}







/*initialize variables for each file to be uncrunched*/
void intram() {
    codlen = 9;	/*    "    */
    fulflg = 0;	/*table empty*/
    entry = 0;	/*    "      */
    entflg = true;	/*first code always atomic*/
}


/*return a code of length "codlen" bits from the input file bit-stream*/
int getcode(content_t *content) {
    int code;
    do {
        code = inBits(content, codlen);
    } while (code == NULCOD || code == SPRCOD);
    return code >= 0 ? code : EOFCOD;
}

/*decode this code*/
bool decodeV1(uint16_t code, content_t *content) {
    if (table[code].suffix ==  EMPTY) {
        /*the ugly exception, "WsWsW"*/
        entflg = true;
        enterxV1(lastpr, finchar);
    }

    /*walk back the lzw table starting with this code*/
    uint8_t stack[TABLE_SIZE];
    uint8_t *stackp = stack;
    while (table[code].predecessor != EMPTY) { /*i.e. code not atomic*/
        *stackp++ = (uint8_t)table[code].suffix;
        code = table[code].predecessor;
        if (stackp >= &stack[TABLE_SIZE]) {
            corrupt = true;
            return(entflg);
        }
    }

    /*then emit all bytes corresponding to this code in forward order*/
    outRle(finchar = table[code].suffix, content); /*first byte*/

    while (stackp > stack)		 /*the rest*/
        outRle(*--stackp, content);

    return(entflg);
}
/*decode this code*/
bool decodeV2(uint16_t code, content_t *content) {

    if (code >= entry) {
        /*the ugly exception, "WsWsW"*/
        entflg = true;
        enterxV2(lastpr, finchar);
    }

    entry_t *ep = &table[code];
    /*mark corresponding table entry as referenced*/
    ep->predecessor |= REFERENCED;

    /*walk back the lzw table starting with this code*/
    uint8_t stack[TABLE_SIZE];
    uint8_t *stackp = stack;
    while (ep > &table[255]) { /*i.e. code not atomic*/
        *stackp++ = (uint8_t)ep->suffix;
        ep = &table[ep->predecessor % TABLE_SIZE];
        if (stackp >= &stack[TABLE_SIZE]) {
            corrupt = true;
            return(entflg);
        }
    }

    /*then emit all bytes corresponding to this code in forward order*/
    outRle(finchar = ep->suffix, content); /*first byte*/

    while (stackp > stack)		 /*the rest*/
        outRle(*--stackp, content);

    return(entflg);
}


/*attempt to reassign an existing code which has*/
/*been defined, but never referenced*/
void entfil(uint16_t pred, uint8_t suff)
// pred     table index of predecessor
// suff     suffix byte represented by this entry
{
    uint16_t hashval = hash(pred, suff);

    /*search the candidate codes (all those which hash from this new*/
    /*predecessor and suffix) for an unreferenced one*/
    for (uint16_t curhash = hashval; xlatbl[curhash] != EMPTY; curhash = (curhash + hashval) % XLATBL_SIZE) {
        /*candidate code*/
        entry_t *ep = table + xlatbl[curhash];
        if (!(ep->predecessor & REFERENCED)) {
            /*entry reassignable, so do it!*/
            ep->predecessor = pred;
            ep->suffix = suff;
            /*discontinue search*/
            break;
        }
    }
}




void initb1() {
    for (int f = 0; f < MAXSTR; f++) {
        xlatbl[f] = IMPRED;
        table[f].suffix = EMPTY;
        table[f].predecessor = EMPTY;
    }

    table[0].predecessor = table[0].suffix = IMPRED;	/* reserved */
    entry = 1;			                                /* since it's a counter, for 1.x */

    for (int f = 0; f < 256; f++)
        enterxV1(0xffff, f);
}


void uncrunchV1(content_t *content) {
    int pred;
    entflg = true;      // 1st code is always atomic
    initb1();

    for (lastpr = 0xffff; (pred = inBits(content, 12)) > 0 && pred < MAXSTR; lastpr = pred) {
        if (decodeV1(pred, content) == false)
            enterxV1(lastpr, finchar);
        else
            entflg = false;

    }
}


void uncrunchV2(content_t *content) {
    intram();   // initialize variables for uncrunching a file
    initb2();   // set up atomic code definitions
    /*main decoding loop*/
    int pred;
    for (lastpr = NOPRED; !corrupt && (pred = getcode(content)) != EOFCOD; lastpr = pred) {
        if (pred == RSTCOD) { /*reset code*/
            entry = 0;
            fulflg = 0;
            codlen = 9;
            pred = NOPRED;
            entflg = true;
            initb2();
        } else if (fulflg != 2) {       /*a normal code - check for table full*/
            if (decodeV2(pred, content) == false)      /*strategy if table not full*/
                enterxV2(lastpr, finchar);
            else
                entflg = false;
        } else {                        /*strategy if table is full*/
            decodeV2(pred, content);
            entfil(lastpr, finchar);    /*attempt to reassign*/
        }
    }
}

/*uncrunch a single file return true for successful uncrunch */
bool uncrunch(content_t *content) {
    unsigned char reflevel;		/*ref rev level from input file*/
    unsigned char siglevel;		/*sig rev level from input file*/
    unsigned char errdetect;	/*error detection flag from input file*/

    if (!parseHeader(content))		// shared header processing
        return false;


    /*read the four info bytes*/
    reflevel = inU8(content);
    siglevel = inU8(content);
    errdetect = inU8(content);
    if (inU8(content) < 0) /*skip spare but check for eof*/
        return false;

    /*make sure we can uncrunch this format file*/
    /*note: this program does not support CRUNCH 1.x format */
    // example 1.1 H:/BEEHIVE/ZCAT/FINDF26.LBR
    if (siglevel < 0x10 || siglevel > 0x2f) {
        printf("%s unsupported version of crunch\n", content->in.fname);
        return false;
    }
    outRle(-1, content);        // reset rle engine
    corrupt = false;            // no corruption detected yet

    if (siglevel >= 0x20)
        uncrunchV2(content);
    else
        uncrunchV1(content);

    if (corrupt)
        return false;

    /*verify checksum if required*/
    int fileCrc = inU16(content);
    if (fileCrc < 0)
        return false;
    if (errdetect == 1)
        return crc16(content->out.buf, content->out.pos) == fileCrc;
    else if (errdetect == 0)
        return crc(content->out.buf, content->out.pos) == fileCrc;
    return true;
}
