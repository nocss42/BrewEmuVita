/*

Copyright (c) 2007-2008  Michael G Schwern

This software originally derived from Paul Sheer's pivotal_gmtime_r.c.

The MIT License:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/
#ifndef TIME64_H
#define TIME64_H

#if defined(__LP64__)

#error Your time_t is already 64-bit.

#else

/* Legacy cruft for LP32 where time_t was 32-bit. */

#include <sys/cdefs.h>
#include <time.h>
#include <stdint.h>

__BEGIN_DECLS

typedef int64_t time64_t;

char* _Nullable asctime64(const struct tm* _Nonnull);
char* _Nullable asctime64_r(const struct tm* _Nonnull, char* _Nonnull);
char* _Nullable ctime64(const time64_t* _Nonnull);
char* _Nullable ctime64_r(const time64_t* _Nonnull, char* _Nonnull);
struct tm* _Nullable gmtime64(const time64_t* _Nonnull);
struct tm* _Nullable gmtime64_r(const time64_t* _Nonnull, struct tm* _Nonnull);
struct tm* _Nullable localtime64(const time64_t* _Nonnull);
struct tm* _Nullable localtime64_r(const time64_t* _Nonnull, struct tm* _Nonnull);
time64_t mktime64(const struct tm* _Nonnull);
time64_t timegm64(const struct tm* _Nonnull);
time64_t timelocal64(const struct tm* _Nonnull);

__END_DECLS

#endif

#endif /* TIME64_H */
