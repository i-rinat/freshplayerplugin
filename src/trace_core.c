/*
 * Copyright © 2013-2017  Rinat Ibragimov
 *
 * This file is part of FreshPlayerPlugin.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "config.h"
#include "trace_core.h"
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static __thread struct timespec tictoc_ts;


void
trace_info(const char *fmt, ...)
{
    if (config.quiet)
        return;
    if (config.quirks.avoid_stdout)
        return;

    pthread_mutex_lock(&lock);
    va_list args;
//    fprintf(stdout, "[fresh] ");
    fprintf(stdout, "[fresh %5d] ", (int)syscall(__NR_gettid));
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
    pthread_mutex_unlock(&lock);
}

void
trace_warning(const char *fmt, ...)
{
    if (config.quirks.avoid_stdout)
        return;

    pthread_mutex_lock(&lock);
    va_list args;
    fprintf(stdout, "[fresh] [warning] ");
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
    pthread_mutex_unlock(&lock);
}

void
trace_error(const char *fmt, ...)
{
    pthread_mutex_lock(&lock);
    va_list args;
    fprintf(stderr, "[fresh] [error] ");
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    if (!config.quirks.avoid_stdout) {
        fprintf(stdout, "[fresh] [error] ");
        va_start(args, fmt);
        vfprintf(stdout, fmt, args);
        va_end(args);
    }
    pthread_mutex_unlock(&lock);
}

void
trace_duration_tic(void)
{
    clock_gettime(CLOCK_REALTIME, &tictoc_ts);
}

double
trace_duration_toc(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (ts.tv_sec - tictoc_ts.tv_sec) + 1e-9 * (ts.tv_nsec - tictoc_ts.tv_nsec);
}
