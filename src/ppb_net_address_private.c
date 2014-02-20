/*
 * Copyright © 2013-2014  Rinat Ibragimov
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

#include <ppapi/c/private/ppb_net_address_private.h>
#include <stddef.h>


const struct PPB_NetAddress_Private_1_1 ppb_net_address_private_interface_1_1 = {
    .AreEqual = (void *)199,
    .AreHostsEqual = (void *)200,
    .Describe = (void *)201,
    .ReplacePort = (void *)202,
    .GetAnyAddress = (void *)203,
    .GetFamily = (void *)204,
    .GetPort = (void *)205,
    .GetAddress = (void *)206,
    .GetScopeID = (void *)207,
    .CreateFromIPv4Address = (void *)208,
    .CreateFromIPv6Address = (void *)209,
};
