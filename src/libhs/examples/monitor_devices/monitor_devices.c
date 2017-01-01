/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Niels Martignène <niels.martignene@gmail.com>
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
 */

#include <inttypes.h>
#include <stdio.h>
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif
#include "../../libhs.h"

static int device_callback(hs_device *dev, void *udata)
{
    (void)(udata);

    const char *event = "?", *type = "?";

    /* Use hs_device_get_status() to differenciate between added and removed devices,
       when called from hs_monitor_list() it is always HS_DEVICE_STATUS_ONLINE. */
    switch (hs_device_get_status(dev)) {
    case HS_DEVICE_STATUS_DISCONNECTED:
        event = "remove";
        break;
    case HS_DEVICE_STATUS_ONLINE:
        event = "add";
        break;
    }

    switch (hs_device_get_type(dev)) {
    case HS_DEVICE_TYPE_HID:
        type = "hid";
        break;
    case HS_DEVICE_TYPE_SERIAL:
        type = "serial";
        break;
    }

    printf("%s %s@%"PRIu8" %04"PRIx16":%04"PRIx16" (%s)\n",
           event, hs_device_get_location(dev), hs_device_get_interface_number(dev),
           hs_device_get_vid(dev), hs_device_get_pid(dev), type);

#define PRINT_PROPERTY(name, prop) \
        if (prop(dev)) \
            printf("  - " name " %s\n", prop(dev));

    PRINT_PROPERTY("device node:  ", hs_device_get_path);
    PRINT_PROPERTY("manufacturer: ", hs_device_get_manufacturer_string);
    PRINT_PROPERTY("product:      ", hs_device_get_product_string);
    PRINT_PROPERTY("serial number:", hs_device_get_serial_number_string);

#undef PRINT_PROPERTY

    /* If you return a non-zero value, the enumeration/refresh is aborted and this value
       is returned from the calling function. */
    return 0;
}

int main(void)
{
    hs_monitor *monitor = NULL;
    hs_poll_source sources[2];
    int r;

    r = hs_monitor_new(NULL, 0, &monitor);
    if (r < 0)
        goto cleanup;

    /* Enumerate devices and start listening to OS notifications. The list is refreshed and the
       callback is called only when hs_monitor_refresh() is called. Use hs_monitor_get_descriptor()
       to get a pollable descriptor and integrate it to your event loop. */
    r = hs_monitor_start(monitor);
    if (r < 0)
        goto cleanup;

    /* hs_monitor_list() uses a cached device list in the monitor object, which is only updated
       when you call hs_monitor_start() and hs_monitor_refresh(). */
    r = hs_monitor_list(monitor, device_callback, NULL);
    if (r < 0)
        goto cleanup;

    /* Add the waitable descriptor provided by the monitor to the descriptor set, it will
       become ready (POLLIN) when there are pending events. */
    sources[0].desc = hs_monitor_get_descriptor(monitor);
    /* We also want to poll the terminal/console input buffer, to exit on key presses. */
#ifdef _WIN32
    sources[1].desc = GetStdHandle(STD_INPUT_HANDLE);
#else
    sources[1].desc = STDIN_FILENO;
#endif

    printf("Monitoring devices (press RETURN to end):\n");
    do {
        /* This function is non-blocking, if there are no pending events it does nothing and
           returns immediately. It calls the callback function for each notification (add or
           remove) and updates the device list accessed by hs_monitor_list(). */
        r = hs_monitor_refresh(monitor, device_callback, NULL);
        if (r < 0)
            goto cleanup;

        /* This function returns the number of ready sources, 0 if it times out or a negative
           error code. You can simply check each source's ready field after each call.  */
        r = hs_poll(sources, 2, -1);
    } while (r > 0 && !sources[1].ready);

    if (sources[1].ready) {
#ifndef _WIN32
        /* Clear the terminal input buffer, just to avoid the extra return/characters from
           showing up when this program exits. This has nothing to do with libhs. */
        tcflush(STDIN_FILENO, TCIFLUSH);
#endif
        r = 0;
    }

cleanup:
    hs_monitor_free(monitor);
    return -r;
}