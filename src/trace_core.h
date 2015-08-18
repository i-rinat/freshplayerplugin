/*
 * Copyright © 2013-2015  Rinat Ibragimov
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

#ifndef FPP_TRACE_CORE_H
#define FPP_TRACE_CORE_H


#define TRACE_WRAPPER       static __attribute__((unused))

// trace non-implemeted or partially implemented function based on build type
#ifndef NDEBUG
// debug build have tracing enabled
#define TWRAPZ(fname)       trace_##fname
#else
#define TWRAPZ(fname)       fname
#endif

// trace implemented function only if TRACE_ALL defined
#ifdef TRACE_ALL
#define TWRAPF(fname)       trace_##fname
#define trace_info_f(...)   trace_info(__VA_ARGS__)
#else
#define TWRAPF(fname)       fname
#define trace_info_f(...)
#endif

#define trace_info_z(...)   trace_info(__VA_ARGS__)


void    trace_info(const char *fmt, ...) __attribute__((format (printf, 1, 2)));
void    trace_warning(const char *fmt, ...) __attribute__((format (printf, 1, 2)));
void    trace_error(const char *fmt, ...) __attribute__((format (printf, 1, 2)));

void    trace_duration_tic(void);
double  trace_duration_toc(void);


#endif // FPP_TRACE_CORE_H
