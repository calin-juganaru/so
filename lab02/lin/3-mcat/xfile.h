/**
 * SO, 2017
 * Lab #2, Operatii I/O simple
 *
 * Task #3, Linux
 *
 * Full length read/write
 */

#ifndef XFILE_UTIL_H
#define XFILE_UTIL_H

#include <unistd.h>

/* Read exactly count bytes or die trying.
 *
 * Return values:
 *  < 0     - error.
 *  >= 0    - number of bytes read.
 */
int xread(int fd, void* buf, int count);

/* Write exactly count bytes or die trying.
 *
 * Return values:
 *  < 0     - error.
 *  >= 0    - number of bytes read.
 */
int xwrite(int fd, const void* buf, int count);

#endif
