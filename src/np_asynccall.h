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

#pragma once

#include <npapi/npapi.h>

// Emulation of NPN_PluginThreadAsyncCall.
//
// There are versions of Firefox, which have NPN_PluginThreadAsyncCall removed.
// Since it's required by FreshPlayerPlugin, it needs to be emulated.
// Current implementation uses the fact the browser runs GLib's mainloop.

/// Initialize plugin-thread-async-call emulation.
///
/// This call must be performed from plugin's main thread.
int
np_asynccall_initialize(void);

/// Schedule |func| on the plugin's main thread.
void
np_asynccall_call(NPP instance, void (*func)(void *), void *userData);
