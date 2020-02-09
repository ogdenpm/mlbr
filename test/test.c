#include "../mlbr.h"


uint8_t type;
uint8_t status;
uint16_t crc;           // used to store squeezed crc
char *srcName;          // name in lbr directory or file name if not lbr file
char *origName;         // name of file pre compression,
char *safeName;         // safe name with illegal chars and reserved names addressed
char *comment;
uint8_t *start;         // location of this file in memory
long length;            // length of file
long pos;               // current position in file
time_t fileDate;


char *testNames[] = {
    "good.asm", "bad/.asm", "bad\\/.asm", "aux", "aux.asm", "com2", "com2./sm", "_aux"
};

xfile_t testFiles[8];

struct {
    char *name;
    uint8_t buf[128];
} test[] = {
    {"squeezed 1", "\x76\xff\0\0hello.asm\0more data"},
{"squeezed 2", "\x76\xff\0\0hellonoext\0more data"},
{"squeezed 3", "\x76\xff\0\0hello.longext\0more data"},
{"squeezed 4", "\x76\xff\0\0with space\0more data"},
{"squeezed 5", "\x76\xff\0\0hello12345678901234567890123456789012345678901234567890123456789.asm\0more data"},
{"crunched 1", "\x76\xfehello.asm\0more data"},
{"crunched 2", "\x76\xfehello.a  \0more data"},
{"crunched 2a", "\x76\xfehello.   \0more data"},
{"crunched 3", "\x76\xfehello.a \0more data"},
{"crunched 4", "\x76\xfehello\0more data"},
{"crunched 5", "\x76\xfehello.asmcomment\0more data"},
{"crunched 6", "\x76\xfehello.a  comment\0more data"},
{"crunched 7", "\x76\xfehello.a  comment\1\x10\x10\x10\x20\x30\x10\x10\x10\x20\x30\x10\x10\x10\x20\x30\0more data"},
{"crunched 8", "\x76\xfehello.a  comment\1\x10\x10\x10\x20\x30\x10\x10\x10\x20\x30\x10\x10\x10\x20\x30[extra]\0more data"},
{"crunched 9", "\x76\xfehello.asm\1\x10\x10\x10\x20\x30\x10\x10\x10\x20\x30\x10\x10\x10\x20\x30\0more data"},
{"crunched 9", "\x76\xfehello.asm\1\x10\x10\x10\x20\x30\x10\x10\x10\x20\x30\x10\x10\x10\x20\x30[extra]\0more data"},
{"crunched 10", "\x76\xfehello.asm[012345678901234567890123456789012345678901234567890123456789]\0more data"},
{"crunched 11", "\x76\xfehello.asm[01234567890123456789]\1\x10\x10\x10\x20\x30\x10\x10\x10\x20\x30\x10\x10\x10\x20\x30\0more data"},
{"crunched 12", "\x76\xfehello.asm[0123456789012\1\x10\x10\x10\x20\x30\x10\x10\x10\x20\x30\x10\x10\x10\x20\x30\0more data"},
    { NULL}
};


void usage(char const *fmt, ...) {
    exit(1);
}
void main() {
    content_t tcontent;
    content_t *p;

    for (int i = 0; test[i].name; i++) {
        memset(&tcontent, 0, sizeof(tcontent));
        if ((p = subContent(&tcontent, test[i].name, test[i].buf, 128, 0)) == NULL)
            printf("could not allocate file for %s\n", test[i].name);
        else {
            printf("%s: ", test[i].name);
            p->type = test[i].buf[1] == 0xfd ? crLzh : test[i].buf[1] == 0xfe ? crunched : squeezed;
            bool result = parseHeader(p);
            if (!result)
                printf("parseHeader returned false\n", test[i].name);
            else
                printf("name: %s,  comment %s, date %lld\n", p->out.fname, p->comment ? p->comment : "(null)", p->out.fdate);
        }
        free(tcontent.in.buf);
        if (tcontent.out.fname) free(tcontent.out.fname);
        if (tcontent.comment) free(tcontent.comment);

    }

}