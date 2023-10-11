/* mlbr - extract .lbr archives and decompress Squeeze, Crunch (v1 & v2)
 *        and Cr-Lzh(v1 & v2) files.
 *	Comments and date stamps are supported as is conversion to .zip file
 *	Copyright (C) - 2020-2023 Mark Ogden
 *
 * showVersion.h - macro interface to showVersion function
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

/* showVersion.h: 2023.5.3.2 [2e11528] */
#ifndef _SHOWVERSION_H_
#define _SHOWVERSION_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CPM  /* don't add overhead to CP/M builds */
#define CHK_SHOW_VERSION(argc, argv)

#else

#include <stdbool.h>
#include <stdio.h>

typedef char const verstr[];
#ifdef _MSC_VER
#ifndef strcasecmp
#define strcasecmp  _stricmp
#endif
#endif

void showVersion(bool full);

#define CHK_SHOW_VERSION(argc, argv)                  \
    if (argc == 2 && strcasecmp(argv[1], "-v") == 0)  \
        do {                                          \
            showVersion(argv[1][1] == 'V');           \
            exit(0);                                  \
    } while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif
