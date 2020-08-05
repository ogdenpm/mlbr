#include <stdio.h>
#include <stdbool.h>
#include <version.h>
// note because version.h is generated in $(intDir) it may not be
// picked up by the editor which will show errors
// build should still be ok

#ifdef GIT_APPID
#define APPNAME GIT_APPID
#else
#define APPNAME GIT_APPDIR
#endif

void showVersion(FILE *fp, bool full) {

    fputs(APPNAME " " GIT_VERSION, fp);
#ifdef _DEBUG
    fputs(" {debug}", fp);
#endif
    fputs("  (C)" GIT_YEAR " Mark Ogden\n", fp);
    if (full) {
        fputs(sizeof(void *) == 4 ? "32bit target" : "64bit target", fp);
        fputs(" Git: " GIT_SHA1 " [" GIT_CTIME " GMT]", fp);
#if GIT_BUILDTYPE == 2
       fputs(" +uncommitted files", fp);
#elif GIT_BUILDTYPE == 3
        fputs("  +untracked files", fp);
#endif
        fputc('\n', fp);
    }
}
