/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Tomas Pecserke (github.com/tomas-pecserke)
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
 * THE SOFTWARE.
 *
 */

#ifndef PICO_TUSB_RESET_INTERFACE_PICO_TUSB_RESET_INTERFACE_H
#define PICO_TUSB_RESET_INTERFACE_PICO_TUSB_RESET_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

// Workaround for missing macros
// TODO remove once https://github.com/hathach/tinyusb/pull/2097 is merged
#ifndef TU_BIT
#define TU_BIT(n)             (1UL << (n))
#endif
#ifndef TU_GENMASK
#define TU_GENMASK(h, l)      ( (UINT32_MAX << (l)) & (UINT32_MAX >> (31 - (h))) )
#endif

#include <stdint.h>
#include <stdbool.h>
// #include <tusb_types.h>
#include <tusb.h>

#ifndef TUD_RPI_RESET_DESC_LEN
// Length of reset vendor interface
#define TUD_RPI_RESET_DESC_LEN  9
#endif

#ifndef TUD_RPI_RESET_DESCRIPTOR
// Reset vendor interface definition template
#define TUD_RPI_RESET_DESCRIPTOR(_itfnum, _stridx) \
  /* Interface */\
  9, TUSB_DESC_INTERFACE, _itfnum, 0, 0, TUSB_CLASS_VENDOR_SPECIFIC, RESET_INTERFACE_SUBCLASS, RESET_INTERFACE_PROTOCOL, _stridx,
#endif

bool reset_interface_cb(uint8_t stage, tusb_control_request_t const * request);

#ifdef __cplusplus
}
#endif

#endif //PICO_TUSB_RESET_INTERFACE_PICO_TUSB_RESET_INTERFACE_H
