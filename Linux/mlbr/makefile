TARGET = mlbr
OBJS = huff.o lzhuf.o main.o memio.o memory.o os.o support.o timegm.o ulbr.o uncrunch.o zip.o zipfile.o
include ../common.mk

huff.o: huff.c mlbr.h
lzhuf.o: lzhuf.c mlbr.h
main.o: main.c mlbr.h showVersion.h
memio.o: memio.c mlbr.h
memory.o: memory.c mlbr.h
os.o: os.c mlbr.h
support.o: support.c timegm.c mlbr.h
ulbr.o: ulbr.c mlbr.h
uncrunch.o: uncrunch.c mlbr.h
zip.o: zip.c miniz.h zip.h
zipfile.o: zipfile.c mlbr.h zip.h
