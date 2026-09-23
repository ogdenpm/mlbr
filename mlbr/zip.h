/* mlbr - extract .lbr archives and decompress Squeeze, Crunch (v1 & v2)
 *        and Cr-Lzh(v1 & v2) files.
 *	Comments and date stamps are supported as is conversion to .zip file
 *	Copyright (C) - 2020-2023 Mark Ogden
 *
 * zip.h - part of zip
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

/* Subset of the Miniz 1.15 code
 * Copyright 2013-2014 RAD Game Tools and Valve Software
 * Copyright 2010-2014 Rich Geldreich and Tenacious Software LLC
 *
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. */

#pragma once
#ifndef ZIP_H
#define ZIP_H

#include <string.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_SSIZE_T_DEFINED) && !defined(_SSIZE_T_DEFINED_) &&               \
    !defined(__DEFINED_ssize_t) && !defined(__ssize_t_defined) &&              \
    !defined(_SSIZE_T) && !defined(_SSIZE_T_) && !defined(_SSIZE_T_DECLARED)

// 64-bit Windows is the only mainstream platform
// where sizeof(long) != sizeof(void*)
#ifdef _WIN64
typedef long long ssize_t; /* byte count or error */
#else
typedef long ssize_t; /* byte count or error */
#endif

#define _SSIZE_T_DEFINED
#define _SSIZE_T_DEFINED_
#define __DEFINED_ssize_t
#define __ssize_t_defined
#define _SSIZE_T
#define _SSIZE_T_
#define _SSIZE_T_DECLARED

#endif

#ifndef MAX_PATH
#define MAX_PATH 32767 /* # chars in a path name including NULL */
#endif

/**
 * @mainpage
 *
 * Documenation for @ref zip.
 */

/**
 * @addtogroup zip
 * @{
 */

/**
 * Default zip compression level.
 */

#define ZIP_DEFAULT_COMPRESSION_LEVEL 6

/**
 * @struct zip_t
 *
 * This data structure is used throughout the library to represent zip archive -
 * forward declaration.
 */
struct zip_t;

/**
 * Opens zip archive with compression level using the given mode.
 *
 * @param zipname zip archive file name.
 * @param level compression level (0-9 are the standard zlib-style levels).
 * @param mode file access mode.
 *        - 'r': opens a file for reading/extracting (the file must exists).
 *        - 'w': creates an empty file for writing.
 *        - 'a': appends to an existing archive.
 *
 * @return the zip archive handler or NULL on error
 */
extern struct zip_t *zip_open(const char *zipname, int level, char mode);

/**
 * Closes the zip archive, releases resources - always finalize.
 *
 * @param zip zip archive handler.
 */
extern void zip_close(struct zip_t *zip);

/**
 * Opens an entry by name in the zip archive.
 *
 * For zip archive opened in 'w' or 'a' mode the function will append
 * a new entry. In readonly mode the function tries to locate the entry
 * in global dictionary.
 *
 * @param zip zip archive handler.
 * @param entryname an entry name in local dictionary.
 *
 * @return the return code - 0 on success, negative number (< 0) on error.
 */
extern int zip_entry_open(struct zip_t *zip, const char *entryname);

/**
 * Closes a zip entry, flushes buffer and releases resources.
 *
 * @param zip zip archive handler.
 * @param m_time timestamp for entry. Zero then ignored
 *
 * @return the return code - 0 on success, negative number (< 0) on error.
 */
extern int zip_entry_close(struct zip_t *zip, time_t m_time);

/**
 * Compresses an input buffer for the current zip entry.
 *
 * @param zip zip archive handler.
 * @param buf input buffer.
 * @param bufsize input buffer size (in bytes).
 *
 * @return the return code - 0 on success, negative number (< 0) on error.
 */
extern int zip_entry_write(struct zip_t *zip, const void *buf, size_t bufsize);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
