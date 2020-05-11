// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// Copyright (c) 2020 Mark Ogden 
#include "mlbr.h"
#include <stdarg.h>
#if 0
uint16_t crc16(uint8_t const *data, long len) {
    uint16_t x;
    uint16_t crc = 0;

    while (len-- > 0) {
        x = (crc >> 8) ^ *data++;
        x ^= x >> 4;
        crc = (crc << 8) ^ (x << 12) ^ (x << 5) ^ x;
    }
    return crc;
}
#else
uint16_t crc16(uint8_t const *data, long len) {
    static unsigned int crc_lookup[256] =
    {
    0x0000,0x1021,0x2042,0x3063,0x4084,0x50A5,0x60C6,0x70E7,
    0x8108,0x9129,0xA14A,0xB16B,0xC18C,0xD1AD,0xE1CE,0xF1EF,
    0x1231,0x0210,0x3273,0x2252,0x52B5,0x4294,0x72F7,0x62D6,
    0x9339,0x8318,0xB37B,0xA35A,0xD3BD,0xC39C,0xF3FF,0xE3DE,
    0x2462,0x3443,0x0420,0x1401,0x64E6,0x74C7,0x44A4,0x5485,
    0xA56A,0xB54B,0x8528,0x9509,0xE5EE,0xF5CF,0xC5AC,0xD58D,
    0x3653,0x2672,0x1611,0x0630,0x76D7,0x66F6,0x5695,0x46B4,
    0xB75B,0xA77A,0x9719,0x8738,0xF7DF,0xE7FE,0xD79D,0xC7BC,
    0x48C4,0x58E5,0x6886,0x78A7,0x0840,0x1861,0x2802,0x3823,
    0xC9CC,0xD9ED,0xE98E,0xF9AF,0x8948,0x9969,0xA90A,0xB92B,
    0x5AF5,0x4AD4,0x7AB7,0x6A96,0x1A71,0x0A50,0x3A33,0x2A12,
    0xDBFD,0xCBDC,0xFBBF,0xEB9E,0x9B79,0x8B58,0xBB3B,0xAB1A,
    0x6CA6,0x7C87,0x4CE4,0x5CC5,0x2C22,0x3C03,0x0C60,0x1C41,
    0xEDAE,0xFD8F,0xCDEC,0xDDCD,0xAD2A,0xBD0B,0x8D68,0x9D49,
    0x7E97,0x6EB6,0x5ED5,0x4EF4,0x3E13,0x2E32,0x1E51,0x0E70,
    0xFF9F,0xEFBE,0xDFDD,0xCFFC,0xBF1B,0xAF3A,0x9F59,0x8F78,
    0x9188,0x81A9,0xB1CA,0xA1EB,0xD10C,0xC12D,0xF14E,0xE16F,
    0x1080,0x00A1,0x30C2,0x20E3,0x5004,0x4025,0x7046,0x6067,
    0x83B9,0x9398,0xA3FB,0xB3DA,0xC33D,0xD31C,0xE37F,0xF35E,
    0x02B1,0x1290,0x22F3,0x32D2,0x4235,0x5214,0x6277,0x7256,
    0xB5EA,0xA5CB,0x95A8,0x8589,0xF56E,0xE54F,0xD52C,0xC50D,
    0x34E2,0x24C3,0x14A0,0x0481,0x7466,0x6447,0x5424,0x4405,
    0xA7DB,0xB7FA,0x8799,0x97B8,0xE75F,0xF77E,0xC71D,0xD73C,
    0x26D3,0x36F2,0x0691,0x16B0,0x6657,0x7676,0x4615,0x5634,
    0xD94C,0xC96D,0xF90E,0xE92F,0x99C8,0x89E9,0xB98A,0xA9AB,
    0x5844,0x4865,0x7806,0x6827,0x18C0,0x08E1,0x3882,0x28A3,
    0xCB7D,0xDB5C,0xEB3F,0xFB1E,0x8BF9,0x9BD8,0xABBB,0xBB9A,
    0x4A75,0x5A54,0x6A37,0x7A16,0x0AF1,0x1AD0,0x2AB3,0x3A92,
    0xFD2E,0xED0F,0xDD6C,0xCD4D,0xBDAA,0xAD8B,0x9DE8,0x8DC9,
    0x7C26,0x6C07,0x5C64,0x4C45,0x3CA2,0x2C83,0x1CE0,0x0CC1,
    0xEF1F,0xFF3E,0xCF5D,0xDF7C,0xAF9B,0xBFBA,0x8FD9,0x9FF8,
    0x6E17,0x7E36,0x4E55,0x5E74,0x2E93,0x3EB2,0x0ED1,0x1EF0,
    };

    uint16_t crc = 0;
    while (len-- > 0)
        crc = (crc << 8) ^ crc_lookup[(crc >> 8) ^ *data++];

    return crc;
}
#endif


uint16_t crc(uint8_t const *data, long len) {
    uint16_t crc = 0;
    while (len-- > 0)
        crc += *data++;
    return crc;
}


int u16At(uint8_t const *buf, long offset) {
    return buf[offset] + buf[offset + 1] * 256;
}



time_t cpmToOsTime(unsigned cpmDay, unsigned timeInSecs) {
    static time_t timeZero;

    if (timeZero == 0) {                            // get CP/M base time for this system
        struct tm timebuf = { 0, 0, 0, 31, 11, 77 };

        timeZero = _mkgmtime(&timebuf);
    }


    if (cpmDay || timeInSecs)
        return timeZero + (time_t)cpmDay * (24 * 3600) + timeInSecs;
    else
        return 0;
}




// mappings, errors and comments are written to a file origname.info if necessary

content_t *addInfoFile(content_t *content) {
    if (content->type == Mapping)
        return content;

    char const *infoName = concat(content->in.fname, ".info", NULL);

    content_t *infoContent = makeDescriptor(&content->in, infoName, NULL, 0);
    infoContent->out = infoContent->in;
    infoContent->type = Mapping;
    if (content->type == Library) {
        infoContent->next = content->lbrHead;
        content->lbrHead = infoContent;
        infoContent->msg = content->msg;     // make library errors appear in its info file
        content->msg = NULL;                // and don't duplicate
    } else {
        infoContent->next = content->next;
        content->next = infoContent;
    }
    return infoContent;
}

static void writeMapping(content_t *infoContent, content_t const *p) {
    bool nameMapped = p->savePath && nameCmp(p->out.fname, nameOnly(p->savePath)) != 0;
    bool skipped = p->type == Skipped || p->type == Missing;

    if (p->msg)
        outStr(infoContent, "%s", p->msg);
    if (p->comment || nameMapped || skipped) {
        outStr(infoContent, "%s", p->out.fname);
        if (nameMapped)
            outStr(infoContent, " <=> %s", nameOnly(p->savePath));
        if (skipped)
            outStr(infoContent, " - not saved");
        if (p->comment) {
            outStr(infoContent, " -- %s", p->comment);
        }
        outStr(infoContent, "\n");
    }
}


void mkOsNames(content_t *content, char const *targetDir, int flags) {
    content_t *infoContent = addInfoFile(content);

    for (content_t *p = content; p; p = p->next) {
        if (p->type == Library) {
            if (flags & EXTRACT) {
                mkOsNames(p->lbrHead, "", flags);
            } else {
                p->out.fname = replaceExt(p->out.fname, "");
                p->savePath = uniqueName(targetDir, p->out.fname);
                writeMapping(infoContent, p);
                mkOsNames(p->lbrHead, p->savePath, flags);
            }
        } else if (p->type != Skipped && p->type != Missing && p->type != Mapping) {
            p->savePath = uniqueName(targetDir, p->out.fname);
            writeMapping(infoContent, p);
        } else
            writeMapping(infoContent, p);
    }
    if (infoContent->out.pos == 0)
        infoContent->type = Skipped;
    else {
        infoContent->savePath = uniqueName(targetDir, infoContent->out.fname);
        writeMapping(infoContent, infoContent);
    }
}


bool parseHeader(content_t *content) {
    uint8_t buf[MAX_HEADER + 4];        // worst case is . at buf[MAX_HEADER - 1], we may set null at buf[MAX_HEADER + 3]
    int c;
    int len;

    inSeek(content, content->type == Squeezed ? 4 : 2);
    for (len = 0; (c = inU8(content)) > 0; len++)
        if (len < MAX_HEADER)
            buf[len] = c;
    if (c < 0)                          // no null
        return false;
    buf[len <= MAX_HEADER ? len : MAX_HEADER] = 0;  // terminate buffer

    if (content->type != Squeezed) {
        uint8_t *stamp;
        if ((stamp = strchr(buf, '.')) && strlen(stamp) > 4) { // if .xxx allow comment/date
            stamp += 4;                                        // standard says .xxx but be flexible
            uint8_t *comment = strchr(stamp, '[');
            if (comment == stamp)
                if (stamp = strchr(comment, ']'))   // allow for date to follow comment
                    stamp++;
                else
                    stamp = strchr(comment, 1);     // or explicit tag

            if (stamp) {                    // stamp -> date stamp if present
                if (*stamp == 1 || *stamp == 'D')            // explict tag
                    *stamp++ = 0;           // if after comment mark end of comment
                if (strlen(stamp) >= 15) {   // comment start is not a valid date field so getCrunchTime will return 0 if there is overlap
                    time_t fdate = getCrunchTime(stamp);
                    if (fdate)
                        content->out.fdate =fdate;
                }
                *stamp = 0;                 // make sure any comment is terminated
            }
            if (comment) {
                content->comment = xstrdup(comment);
                *comment = 0;
            }
        }
    }
    uint8_t *s;
    // although chars < ' ' are not meant to be in the file name
    // one example I have seen has \b chars after the 3rd ext char
    // so some extra code to truncate ext if it contains <= ' '
    if (s = strchr(buf, '.')) {
        while (*++s && (*s & 0x7f) >= ' ')
            ;
        *s = 0;
    }
    for (s = buf; *s; s++)
        if ((*s &= 0x7f) < ' ')    // illegal name so assume not compressed file
            return false;
    if (--s >= buf && *s == '.')                 // remove null ext
        *s = 0;
    content->out.fname = mapCase(xstrdup(buf));
    if (len > MAX_HEADER)
        printf("Warning: %s header truncated\n", content->in.fname);
    return true;
}


void logErr(content_t *content, char const *fmt, ...) {

    va_list args;
    va_start(args, fmt);
    int  msgLen = _vscprintf(fmt, args);                    // length of new message
    msgLen += (int)(content->msg ? strlen(content->msg) : 0) + 1; // allow for \0
    char *msg = sAlloc(msgLen);
    strcpy(msg, content->msg ? content->msg : "");
    char *s = strchr(msg, '\0');     // append point for new msg
    vsprintf(s, fmt, args);
    va_end(args);
    content->msg = msg;
}


// utility function to concat strings, last arg is NULL to signal end
char const *concat(const char *s, ...) {
    size_t slen = strlen(s) + 1;       // length of final string, init with first string & '\0'
    va_list args;
    char const *t;

    va_start(args, s);          // get length of final string
    while (t = va_arg(args, char const *))
        slen += strlen(t);
    va_end(args);

    char *cStr = sAlloc(slen);
    strcpy(cStr, s);
    va_start(args, s);          // now build the rest of the string
    while (t = va_arg(args, char const *))
        strcat(cStr, t);
    va_end(args);
    return cStr;
}
