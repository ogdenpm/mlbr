#include "mlbr.h"

#define MAXNODE 256

static struct {
    int child[2];
} node[MAXNODE + 1];


static int usqU8(content_t *content) {
    int i;
    int cbit;

    for (i = 0; i >= 0 && (cbit = inBitRev(content)) >= 0;)
        i = node[i].child[cbit];
    
    i = -(i + 1);
    return (cbit < 0 || i == MAXNODE) ? EOF : i;
}




bool unsqueeze(content_t *content) {
    int nodeCnt;
    int c;

    if (!parseHeader(content))
        return false;

    nodeCnt = inU16(content);
    if (nodeCnt < 0 || nodeCnt > MAXNODE)
        return false;
    // put in minimal node (EOF)
    node[0].child[0] = node[0].child[1] = -(MAXNODE + 1);
    
    for (int i = 0; i < nodeCnt; i++) {
        node[i].child[0] = inI16(content);
        node[i].child[1] = inI16(content);
    }
    if (isEof(content))
        return false;

    outRle(-1, content);           // reset engine
    while ((c = usqU8(content)) != EOF)
        outRle(c, content);

    inSeek(content, 2);                 // locate the CRC
    /*verify checksum*/
    return crc(content->out.buf, content->out.pos) == inU16(content);
}

