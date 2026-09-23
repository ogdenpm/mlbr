/* mlbr - extract .lbr archives and decompress Squeeze, Crunch (v1 & v2)
 *        and Cr-Lzh(v1 & v2) files.
 *	Comments and date stamps are supported as is conversion to .zip file
 *	Copyright (C) - 2020-2023 Mark Ogden
 *
 * huff.c - implements huff decompression
 *
 * NOTE: Elements of the code have been derived from public shared
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

#define MAXNODE 256

static struct { int child[2]; } node[MAXNODE + 1];

static int usqU8(content_t *content) {
    int i;
    int cbit;

    for (i = 0; i >= 0 && (cbit = inBitRev(content)) >= 0;) {
        i = node[i].child[cbit];
    }

    i = -(i + 1);
    return (cbit < 0 || i == MAXNODE) ? EOF : i;
}

int unsqueeze(content_t *content) {
    int nodeCnt;
    int c;

    if (!parseHeader(content)) {
        return BADHEADER;
    }

    nodeCnt = inU16(content);
    if (nodeCnt < 0 || nodeCnt > MAXNODE) {
        return BADHEADER;
    }
    // put in minimal node (EOF)
    node[0].child[0] = node[0].child[1] = -(MAXNODE + 1);

    for (int i = 0; i < nodeCnt; i++) {
        node[i].child[0] = inI16(content);
        node[i].child[1] = inI16(content); //-V656
    }
    if (isEof(content)) {
        return CORRUPT;
    }

    outRle(-1, content); // reset engine
    while ((c = usqU8(content)) != EOF) {
        outRle(c, content);
    }

    inSeek(content, 2); // locate the CRC
    /*verify checksum*/
    return crc(content->out.buf, content->out.pos) == inU16(content); // returns BADCRC or GOOD
}
