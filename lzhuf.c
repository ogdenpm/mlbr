/* mlbr - extract .lbr archives and decompress Squeeze, Crunch (v1 & v2)
 *        and Cr-Lzh(v1 & v2) files.
 *	Comments and date stamps are supported as is conversion to .zip file
 *	Copyright (C) - 2020-2023 Mark Ogden
 *
 * lzhuf.c - implementation of llzhuf
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


/* Original source Copyright  nformation
 * LZHUF.C English version 1.0
 * Based on Japanese version 29-NOV-1988
 * LZSS coded by Haruhiko OKUMURA
 * Adaptive Huffman Coding coded by Haruyasu YOSHIZAKI
 * Edited and translated to English by Kenji RIKITAKE

   Modifications to integrate into mlbr by Mark Ogden 2-Feb-2020
   removed encode functions
   converted bit masking to % (mod) operations - let the compiler work out what is best

 */

#include "mlbr.h"

/* LZSS Parameters */

/* these are the values required for the "Y" format */
#define LZ_N      2048
#define LZ_F      60
#define THRESHOLD 2

#define EOF_CODE  256

static uint8_t text_buf[LZ_N + LZ_F - 1];

/* Huffman coding parameters */
#define N_CHAR   (256 + 1 - THRESHOLD + LZ_F)
/* kinds of characters (character code = 0..N_CHAR-1) */
#define LZ_T     (N_CHAR * 2 - 1) /* size of table */
#define LZ_R     (LZ_T - 1)       /* position of root */
#define MAX_FREQ 0x8000           /* updates tree when the */
                                  /* root frequency comes to this value. */

static uint8_t oldver;

/*
 * Tables for decoding upper 6 bits of
 * sliding dictionary pointer
 */

/* decoder table */
static uint8_t d_code[256] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
    0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0D, 0x0E, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F,
    0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x13,
    0x14, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15, 0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17,
    0x18, 0x18, 0x19, 0x19, 0x1A, 0x1A, 0x1B, 0x1B, 0x1C, 0x1C, 0x1D, 0x1D, 0x1E, 0x1E, 0x1F, 0x1F,
    0x20, 0x20, 0x21, 0x21, 0x22, 0x22, 0x23, 0x23, 0x24, 0x24, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27,
    0x28, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2B, 0x2B, 0x2C, 0x2C, 0x2D, 0x2D, 0x2E, 0x2E, 0x2F, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
};

static uint8_t d_len[256] = {
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
};

static unsigned freq[LZ_T + 1]; /* cumulative freq table */

/*
 * pointing parent nodes.
 * area [LZ_T..(LZ_T + N_CHAR - 1)] are pointers for leaves
 */
static int prnt[LZ_T + N_CHAR];

/* pointing children nodes (son[], son[] + 1)*/
static int son[LZ_T + 1]; // getcode could access son[LZ_T]

/* initialize freq tree */

static void startHuff() {

    for (int i = 0; i < N_CHAR; i++) {
        freq[i]        = 1;
        son[i]         = i + LZ_T;
        prnt[i + LZ_T] = i;
    }
    for (int i = 0, j = N_CHAR; j <= LZ_R; i += 2, j++) {
        freq[j] = freq[i] + freq[i + 1];
        son[j]  = i;
        prnt[i] = prnt[i + 1] = j;
    }
    freq[LZ_T] = 0xffff;
    prnt[LZ_R] = 0;
}

/* reconstruct freq tree */

static void reconst() {
    /* halven cumulative freq for leaf nodes */

    for (int i = 0, j = 0; i < LZ_T; i++) {
        if (son[i] >= LZ_T) {
            freq[j] = (freq[i] + 1) / 2;
            son[j]  = son[i];
            j++;
        }
    }
    /* make a tree : first, connect children nodes */
    for (int i = 0, j = N_CHAR; j < LZ_T; i += 2, j++) {
        unsigned f = freq[i] + freq[i + 1];
        int k;
        for (k = j; f < freq[k - 1]; k--) { // find insert point
            freq[k] = freq[k - 1];          // by moving items up
            son[k]  = son[k - 1];
        }
        freq[k] = f; // insert
        son[k]  = i;
    }
    /* connect parent nodes */
    for (int i = 0; i < LZ_T; i++) {
        int k;
        if ((k = son[i]) >= LZ_T) {
            prnt[k] = i;
        } else {
            prnt[k] = prnt[k + 1] = i;
        }
    }
}

/* update freq tree */

static void update(unsigned c) {
    unsigned i;
    unsigned j;
    unsigned k;
    unsigned l;

    if (freq[LZ_R] == MAX_FREQ) {
        reconst();
    }

    c = prnt[c + LZ_T];
    do {
        k = ++freq[c];

        /* swap nodes to keep the tree freq-ordered */
        if (k > freq[l = c + 1]) {
            while (k > freq[++l]) {
                ;
            }
            l--;
            freq[c] = freq[l];
            freq[l] = k;

            i       = son[c];
            prnt[i] = l;
            if (i < LZ_T) {
                prnt[i + 1] = l;
            }

            j       = son[l];
            son[l]  = i;

            prnt[j] = c;
            if (j < LZ_T) {
                prnt[j + 1] = c;
            }
            son[c] = j;

            c      = l;
        }
    } while ((c = prnt[c]) != 0); /* do it until reaching the root */
}

static unsigned DecodeChar(content_t *content) {
    unsigned c;

    c = son[LZ_R];

    /*
     * start searching tree from the root to leaves.
     * choose node #(son[]) if input bit == 0
     * else choose #(son[]+1) (input bit == 1)
     */
    while (c < LZ_T) {
        c = son[c + (inBits(content, 1) > 0)]; // will map <= 0 to 0 else 1
    }
    c -= LZ_T;
    update(c);
    return c;
}

static unsigned DecodePosition(content_t *content) {
    int i;
    unsigned j;
    unsigned c;

    /* decode upper 6 bits from given table */
    if ((i = inBits(content, 8)) < 0) {
        i = 0;
    }

    c = d_code[i] << (5 + oldver); // 5 or 6 for 1.x

    /* input lower 6 bits directly */
    j = d_len[i] - (3 - oldver); // 3 or 2 for 1.x

    while (j--) {
        i = (i << 1) + (inBits(content, 1) > 0);
    }
    return c | (i & (oldver ? 0x3f : 0x1f)); // 0x1f or 0x3f for 1.x
}

int uncrLzh(content_t *content) { /* Decoding/Uncompressing */
    unsigned c;
    unsigned i;
    unsigned j;
    unsigned k;
    unsigned r;
    uint8_t reflevel;  /*ref rev level from input file*/
    uint8_t siglevel;  /*sig rev level from input file*/
    uint8_t errdetect; /*error detection flag from input file*/

    if (!parseHeader(content)) {
        return BADHEADER;
    }
    reflevel  = inU8(content);
    siglevel  = inU8(content);
    errdetect = inU8(content);
    if (inU8(content) < 0) { // skip spare but check for eof
        return BADHEADER;
    }

    if (siglevel < 0x10 || siglevel > 0x2f) {
        return BADHEADER;
    }

    oldver        = siglevel < 0x20;
    content->type = siglevel < 0x20 ? CrLzhV1 : CrLzhV2;

    startHuff();
    r = LZ_N - LZ_F;
    memset(text_buf, ' ', r); //-V512

    // if we reach EOF then we don't have the CRC info
    while ((c = DecodeChar(content)) != EOF_CODE &&
           !isEof(content)) { // EOF or no more bytes (need 2 for CRC)
        if (c < EOF_CODE) {
            outU8(c, content);
            text_buf[r++] = c;
            r %= LZ_N;
        } else {
            i = (r - DecodePosition(content) - 1) % LZ_N;
            j = c - EOF_CODE + THRESHOLD;
            for (k = 0; k < j; k++) {
                c = text_buf[(i + k) % LZ_N];
                outU8(c, content);
                text_buf[r++] = c;
                r %= LZ_N;
            }
        }
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
