/* mlbr - extract .lbr archives and decompress Squeeze, Crunch (v1 & v2)
 *        and Cr-Lzh(v1 & v2) files.
 *	Comments and date stamps are supported as is conversion to .zip file
 *	Copyright (C) - 2020-2023 Mark Ogden
 *
 * _version.c - show version information
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

/*
 * showVersion, simple function to display version information
 */
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>
#include "showVersion.h"

#include "_version.h"       // generated by getVersion
#include "appinfo.h"        // static version info
// showVersion: 2023.9.30.16 [82f79ab]
//
// in the unlikely case that APP_PRODUCT and APP_OWNER are not
// defined given them sensible values
#ifndef APP_PRODUCT
#define APP_PRODUCT APP_NAME
#endif
#ifndef APP_OWNER
#define APP_OWNER   "Mark Ogden"
#endif

#ifdef _DEBUG
#define BUILD "debug "
#else
#define BUILD
#endif


/* if common libraries are used then define APP_LIBS with a list
 * of the version strings exported by the libraries
 * these version strings are arrays of char
 */
#ifdef APP_LIBS
extern verstr APP_LIBS;
char const *libvers[] = { APP_LIBS, NULL };
#endif

/// <summary>
/// Shows the version information combining the static information from appinfo.h
/// and the generated version information derived from git in _version.h
/// </summary>
/// <param name="full">if set to <c>true</c>Display extended version info.</param>
/// Common usage is to call from main
/*
 * #include "showVersion.h
 *
 * and in main code
 * CHK_SHOW_VERSION(argc, argv);
 *
*/

void showVersion(bool full) {

    puts(APP_PRODUCT " (C) " APP_OWNER
#ifdef APP_MODS
          " <" APP_MODS ">"
#endif
          " " GIT_VERSION);
    if (full) {    // full version info
#ifdef APP_DESCRIPTION
        puts(APP_DESCRIPTION);
#endif


#ifdef APP_CONTRIBUTOR
        puts("Contributors: " APP_CONTRIBUTOR);
#endif
        printf("%d bit " BUILD "build: " __DATE__ " " __TIME__  "\n",
                (int)(sizeof(void *) * CHAR_BIT));
#ifdef APP_LIBS
        for (char const **p = libvers; *p; p++) {
            putchar('+');
            putchar(' ');
            puts(*p);
        }
#endif
#ifdef APP_EMAIL
        puts("Support email: " APP_EMAIL);
#endif
    }
}
