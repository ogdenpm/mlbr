// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// Copyright (c) 2020 Mark Ogden 
#include "mlbr.h"
#include <ctype.h>

/*
    lbr time is encoded as follows
    createDay   16bit le    days from 31/12/1977 i.e. 1  -> 1/1/1980
    changeDay   16bit le
    createTime  16it  le    DOS format hhhhh mmmmmm sssss where ssss is seconds / 2
    changeTime  16bit le
*/
static time_t getLbrTime(uint8_t const *lbrItem) {
    unsigned lbrDay;
    unsigned lbrTime;

    if (lbrDay = u16At(lbrItem, ChangeDate))     // use modify time if valid
        lbrTime = u16At(lbrItem, ChangeTime);
    else {
        lbrDay = u16At(lbrItem, CreateDate);     //  else create date
        lbrTime = u16At(lbrItem, ChangeTime);
    }

    if (lbrDay) {
        lbrTime = (lbrTime >> 11) * 3600 + ((lbrTime >> 5) & 0x3f) * 60 + (lbrTime & 0x1f) * 2; // convert to seconds
        return cpmToOsTime(lbrDay, lbrTime);
    }
    return 0;
}

static char *getLbrName(uint8_t const *lbrItem) {
    static char name[13];
    char *s = name;
    for (int i = Name; i < Name + 8; i++, s++)
        if ((*s = lbrItem[i] & 0x7f) == ' ')
            break;
    if ((lbrItem[Ext] & 0x7f) != ' ') {
        *s++ = '.';
        for (int i = Ext; i < Ext + 3; i++, s++)
            if ((*s = lbrItem[i] & 0x7f) == ' ')
                break;
    }
    *s = 0;
    return strdup(mapCase(name));
}


bool parseLbr(content_t *content) {
    uint8_t *lbrBuf = content->in.buf;
    long dirSize = u16At(lbrBuf, Length) * 128;

    if (dirSize > content->in.bufSize)
        return false;

    uint16_t crc = u16At(lbrBuf, Crc);

    lbrBuf[Crc] = 0;                   // for directories CRC is handled specially (replace CRC with 0)
    lbrBuf[Crc + 1] = 0;

    if (crc16(lbrBuf, dirSize) != crc)
        content->status |= (crc && crc != 0xffff) ? BADCRC : NOCRC;
    
    for (int off = dirSize - LBRDIR_SIZE; off > 0; off -= LBRDIR_SIZE) {        // process backwards as chain inserts at front
        if ((lbrBuf + off)[Status] == 0) {
            uint8_t *start = lbrBuf + u16At(lbrBuf + off, Index) * 128;
            long length = u16At(lbrBuf + off, Length) * 128;

            content_t *descriptor = makeDescriptor(&content->in, getLbrName(lbrBuf + off), start, length);
            descriptor->in.fdate = descriptor->out.fdate = getLbrTime(lbrBuf + off);  // use the lbr directory date may be over written by crunch date
            descriptor->status |= INCONTAINER;
            descriptor->next = content->next;           // insert in chain
            content->next = descriptor;


            crc = u16At(lbrBuf + off, Crc);
            if (crc16(descriptor->in.buf, descriptor->in.bufSize) != crc)
                descriptor->status |= (crc && crc != 0xffff) ? BADCRC : NOCRC;

            // pad count adjustment is NOT done since in the files I have seen
            // is isn't reliable and sometimes invalid
            // for compressed files is not needed as the
            // decoders will stop at the internal end marker
        }

    }
    // TODO - check if any of the content overlaps - unlikely to be implemented
    return true;
}

