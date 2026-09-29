# mlbr
Lbr archive utility

Simple utility to read lbr, squeeze, crunch and lzh files

For windows there is a visual studio solution file or alternatively the Microsoft Visual C compiler can be invoked from the development command prompt by using

```
cl /Fe:mlbr.exe *.c setargv.obj
```

For compilation on non Windows systems there is a makefile in the Linux directory that can be used to build mlbr. It has been tested using WSL (Ubuntu) on Windows by myself and on AIX, Solaris, Haiku, Illumos, and several flavours of BSD by Jeffrey Johnson.
The makefile is written for gnu make, and the C compiler and flags can be overridden by setting the CC and CLFAG variables on the make command line. The compiled file is built in the Linux/Install directory. See the makefile for the various build targets.

```
Usage: mlbr -v | -V | [-x | -d | -z]  [-D dir] [-f] [-i] [-k] [-n] [-r] [--] file+
   -v / -V show version information and exit
   -x  extract to directory
   -d  extract lbr to sub directory {name} - see below
   -z  convert to zip file {name}.zip
   -D  override target directory
   -f  forces write of skipped library content
   -i  ignore crc errors
   -I  ignore crc errors and corrupt decompression
   -k  keep original case of file names (default is to lower case)
   -n  don't expand compressed files
   -r  recursively extract lbr, creates nested sub directories for -d or -z
   -v  show verison details and exit
   --  terminates args to support files with a leading -
 
 file+ one or more lbr, squeezed, crunched or crLzhed files
       names can include * or ? wildcard characters
 {name} is file with leading directory and extent removed

 Listing of file details, including validation checks is always done
 The -x or -d options allow files to be extracted
 
 When files contain comments or are renamed to avoid conflicts
 the details are written to a file {name}.info
```

Mark

29-Sept-2026
