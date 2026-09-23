/* mlbr - extract .lbr archives and decompress Squeeze, Crunch (v1 & v2)
 *        and Cr-Lzh(v1 & v2) files.
 *	Comments and date stamps are supported as is conversion to .zip file
 *	Copyright (C) - 2020-2023 Mark Ogden
 *
 * ulbr.c - handles unpacking lbr file
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
#include <ctype.h>

/*
    lbr time is encoded as follows
    createDay   16bit le    days from 31/12/1977 i.e. 1  -> 1/1/1978
    changeDay   16bit le
    createTime  16it  le    DOS format hhhhh mmmmmm sssss where sssss is seconds / 2
    changeTime  16bit le
*/
/* as dates were added to CP/M after the epoch and LBR is mainly for historic use
   these defines allow for valid range checking
   the current values are very broad and allow for LBR to add dated files for several years
   also allow some historic dates to be added
   they could be tightend up to detect more erroneous dates
*/
#define MAXDAY (2030 - 1978) * 365 // ~2030 (excluding leap years)
#define MINDAY  1                  // date recording came after epoch so 0 is not valid

static time_t getLbrTime(uint8_t const *lbrItem) {
    unsigned lbrDay;
    unsigned lbrTime;

    if ((lbrDay = u16At(lbrItem, ChangeDate))) { // use modify time if valid
        lbrTime = u16At(lbrItem, ChangeTime);
    } else {
        lbrDay  = u16At(lbrItem, CreateDate); //  else create date
        lbrTime = u16At(lbrItem, CreateTime);
    }
    // as lbr is for historic use and the time fields were added later
    // check for plausible date/time values, otherwise ignore
    if (MINDAY <= lbrDay && lbrDay < MAXDAY && (lbrTime >> 11) < 24 && ((lbrTime >> 5) & 0x3f) < 60 &&
        (lbrTime & 0x1f) < 30) {

        lbrTime = (lbrTime >> 11) * 3600 + ((lbrTime >> 5) & 0x3f) * 60 +
                  (lbrTime & 0x1f) * 2; // convert to seconds
        return cpmToOsTime(lbrDay, lbrTime);
    }
    return 0;
}

static char *getLbrName(uint8_t const *lbrItem) {
    static char name[13];
    char *s = name;
    for (int i = Name; i < Name + 8; i++, s++) {
        if ((*s = lbrItem[i] & 0x7f) == ' ') {
            break;
        }
    }
    if ((lbrItem[Ext] & 0x7f) != ' ') {
        *s++ = '.';
        for (int i = Ext; i < Ext + 3; i++, s++) {
            if ((*s = lbrItem[i] & 0x7f) == ' ') {
                break;
            }
        }
    }
    *s = 0;
    return xstrdup(mapCase(name));
}

bool parseLbr(content_t *content) {
    uint8_t *lbrBuf = content->in.buf;
    long dirSize    = u16At(lbrBuf, Length) * 128;

    if (dirSize > content->in.bufSize) {
        return false;
    }

    uint16_t crc    = u16At(lbrBuf, Crc);

    lbrBuf[Crc]     = 0; // for directories CRC is handled specially (replace CRC with 0)
    lbrBuf[Crc + 1] = 0;

    if (crc16(lbrBuf, dirSize) != crc) {
        content->status |= (crc && crc != 0xffff) ? F_BADCRC : F_NOCRC;
        logErr(content, "!! %s library CRC is %s\n", content->in.fname,
               (content->status & F_BADCRC) ? "bad" : "missing");
    }
    content->out.fdate = getLbrTime(lbrBuf);

    for (long off = dirSize - LBRDIR_SIZE; off > 0;
         off -= LBRDIR_SIZE) { // process backwards as chain inserts at front
        if ((lbrBuf + off)[Status] == 0) {
            uint8_t *start = lbrBuf + (size_t)u16At(lbrBuf + off, Index) * 128;
            long length    = u16At(lbrBuf + off, Length) * 128;

            content_t *descriptor =
                makeDescriptor(&content->in, getLbrName(lbrBuf + off), start, length);
            descriptor->in.fdate = descriptor->out.fdate = getLbrTime(
                lbrBuf + off); // use the lbr directory date may be over written by crunch date
            descriptor->next = content->lbrHead; // insert in chain
            content->lbrHead = descriptor;

            crc              = u16At(lbrBuf + off, Crc);
            if (crc16(descriptor->in.buf, descriptor->in.bufSize) != crc) {
                descriptor->status |= (crc && crc != 0xffff) ? F_BADCRC : F_NOCRC;
            }

            // pad count adjustment is NOT done since in the files I have seen
            // is isn't reliable and sometimes invalid
            // for compressed files is not needed as the
            // decoders will stop at the internal end marker
        }
    }
    // TODO - check if any of the content overlaps - unlikely to be implemented
    return true;
}
