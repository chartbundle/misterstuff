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

#include "pico_tusb_reset_interface.h"

#include <pico/stdio_usb.h>
#define PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE 1

#if PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE && !(PICO_STDIO_USB_RESET_INTERFACE_SUPPORT_RESET_TO_BOOTSEL || PICO_STDIO_USB_RESET_INTERFACE_SUPPORT_RESET_TO_FLASH_BOOT)
#warning PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE has been selected but neither PICO_STDIO_USB_RESET_INTERFACE_SUPPORT_RESET_TO_BOOTSEL nor PICO_STDIO_USB_RESET_INTERFACE_SUPPORT_RESET_TO_FLASH_BOOT have been selected.
#endif

#if PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE
#include <device/usbd.h>
#include <pico/bootrom.h>
#include <pico/stdio_usb/reset_interface.h>
#include <hardware/watchdog.h>

// Support for parameterized reset via vendor interface control request
bool reset_interface_cb(uint8_t stage, tusb_control_request_t const * request) {
    // nothing to do with DATA & ACK stage
    if (stage != CONTROL_STAGE_SETUP) return true;
#if PICO_STDIO_USB_RESET_INTERFACE_SUPPORT_RESET_TO_BOOTSEL
    if (request->bRequest == RESET_REQUEST_BOOTSEL) {
#ifdef PICO_STDIO_USB_RESET_BOOTSEL_ACTIVITY_LED
        uint gpio_mask = 1u << PICO_STDIO_USB_RESET_BOOTSEL_ACTIVITY_LED;
#else
        uint gpio_mask = 0u;
#endif
#if !PICO_STDIO_USB_RESET_BOOTSEL_FIXED_ACTIVITY_LED
        if (request->wValue & 0x100) {
            gpio_mask = 1u << (request->wValue >> 9u);
        }
#endif
        reset_usb_boot(gpio_mask, (request->wValue & 0x7f) | PICO_STDIO_USB_RESET_BOOTSEL_INTERFACE_DISABLE_MASK);
        // does not return, otherwise we'd return true
    }
#endif

#if PICO_STDIO_USB_RESET_INTERFACE_SUPPORT_RESET_TO_FLASH_BOOT
    if (request->bRequest == RESET_REQUEST_FLASH) {
        watchdog_reboot(0, 0, PICO_STDIO_USB_RESET_RESET_TO_FLASH_DELAY_MS);
        return true;
    }
#endif
    return false;
}
#endif
