// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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
