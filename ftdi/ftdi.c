/***************************************************************************
                          ftdi.c  -  description
                             -------------------
    begin                : Fri Apr 4 2003
    copyright            : (C) 2003 by Intra2net AG
    email                : info@intra2net.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License           *
 *   version 2.1 as published by the Free Software Foundation;             *
 *                                                                         *
 ***************************************************************************/

#include <usb.h>
 
#include "ftdi.h"

int ftdi_init(struct ftdi_context *ftdi) {
    ftdi->usb_dev = NULL;
    ftdi->usb_timeout = 5000;

    ftdi->baudrate = -1;
    ftdi->bitbang_enabled = 0;

    ftdi->error_str = NULL;

    return 0;
}


void ftdi_set_usbdev (struct ftdi_context *ftdi, usb_dev_handle *usb) {
    ftdi->usb_dev = usb;
}


/* ftdi_usb_open return codes:
   0: all fine
  -1: usb_find_busses() failed
  -2: usb_find_devices() failed
  -3: usb device not found
  -4: unable to open device
  -5: unable to claim device
  -6: reset failed
  -7: set baudrate failed
*/
int ftdi_usb_open(struct ftdi_context *ftdi, int vendor, int product) {
    struct usb_bus *bus;
    struct usb_device *dev;

    usb_init();

    if (usb_find_busses() < 0) {
        ftdi->error_str = "usb_find_busses() failed";
        return -1;
    }

    if (usb_find_devices() < 0) {
        ftdi->error_str = "usb_find_devices() failed";
        return -2;
    }

    for (bus = usb_busses; bus; bus = bus->next) {
        for (dev = bus->devices; dev; dev = dev->next) {
            if (dev->descriptor.idVendor == vendor && dev->descriptor.idProduct == product) {
                ftdi->usb_dev = usb_open(dev);
                if (ftdi->usb_dev) {
                    if (usb_claim_interface(ftdi->usb_dev, 0) != 0) {
                        ftdi->error_str = "unable to claim usb device. You can still use it though...";
                        return -5;
                    }

                    if (ftdi_usb_reset (ftdi) != 0)
                       return -6;

                    if (ftdi_set_baudrate (ftdi, 9600) != 0)
                       return -7;

                    return 0;
                } else {
                    ftdi->error_str = "usb_open() failed";
                    return -4;
                }
            }
        }

    }

    // device not found
    return -3;
}


int ftdi_usb_reset(struct ftdi_context *ftdi) {
    if (usb_control_msg(ftdi->usb_dev, 0x40, 0, 0, 0, NULL, 0, ftdi->usb_timeout) != 0) {
        ftdi->error_str = "FTDI reset failed";
        return -1;
    }

    return 0;
}


/* ftdi_usb_close return codes
    0: all fine
   -1: usb_release failed
   -2: usb_close failed
*/
int ftdi_usb_close(struct ftdi_context *ftdi) {
    int rtn = 0;

    if (usb_release_interface(ftdi->usb_dev, 0) != 0)
        rtn = -1;

    if (usb_close (ftdi->usb_dev) != 0)
        rtn = -2;

    return rtn;
}


/*
    ftdi_set_baudrate return codes:
     0: all fine
    -1: invalid baudrate
    -2: setting baudrate failed
*/
int ftdi_set_baudrate(struct ftdi_context *ftdi, int baudrate) {
    unsigned short ftdi_baudrate;

    if (ftdi->bitbang_enabled) {
        baudrate = baudrate*4;
    }

    switch (baudrate) {
    case 300:
        ftdi_baudrate = 0x2710;
        break;
    case 600:
        ftdi_baudrate = 0x1388;
        break;
    case 1200:
        ftdi_baudrate = 0x09C4;
        break;
    case 2400:
        ftdi_baudrate = 0x04E2;
        break;
    case 4800:
        ftdi_baudrate = 0x0271;
        break;
    case 9600:
        ftdi_baudrate = 0x4138;
        break;
    case 19200:
        ftdi_baudrate = 0x809C;
        break;
    case 38400:
        ftdi_baudrate = 0xC04E;
        break;
    case 57600:
        ftdi_baudrate = 0x0034;
        break;
    case 115200:
        ftdi_baudrate = 0x001A;
        break;
    case 230400:
        ftdi_baudrate = 0x000D;
        break;
    case 460800:
        ftdi_baudrate = 0x4006;
        break;
    case 921600:
        ftdi_baudrate = 0x8003;
        break;
    default:
        ftdi->error_str = "Unknown baudrate. Note: bitbang baudrates are automatically multiplied by 4";
        return -1;
    }

    if (usb_control_msg(ftdi->usb_dev, 0x40, 3, ftdi_baudrate, 0, NULL, 0, ftdi->usb_timeout) != 0) {
        ftdi->error_str = "Setting new baudrate failed";
        return -2;
    }

    ftdi->baudrate = baudrate;
    return 0;
}


int ftdi_write_data(struct ftdi_context *ftdi, char *buf, int size) {
    int ret;
    int offset = 0;
    while (offset < size) {
        int write_size = 64;

        if (offset+write_size > size)
            write_size = size-offset;

        ret=usb_bulk_write(ftdi->usb_dev, 2, buf+offset, write_size, ftdi->usb_timeout);
        if (ret == -1)
            return -1;

        offset += write_size;
    }

    return 0;
}


int ftdi_read_data(struct ftdi_context *ftdi, char *buf, int size) {
    /*
      unsigned char buf[64];
      int read_bytes;

      read_bytes = usb_bulk_read (udev, 0x81, (char *)&buf, 64, USB_TIMEOUT);
    */
    ftdi->error_str = "Not implemented yet";
    return -1;
}


int ftdi_enable_bitbang(struct ftdi_context *ftdi, unsigned char bitmask) {
    unsigned short usb_val;

    usb_val = bitmask;	// low byte: bitmask
    usb_val += 1 << 8;	// high byte: enable flag
    if (usb_control_msg(ftdi->usb_dev, 0x40, 0x0B, usb_val, 0, NULL, 0, ftdi->usb_timeout) != 0) {
        ftdi->error_str = "Unable to enter bitbang mode. Perhaps not a BM type chip?";
        return -1;
    }

    ftdi->bitbang_enabled = 1;
    return 0;
}


int ftdi_disable_bitbang(struct ftdi_context *ftdi) {
    if (usb_control_msg(ftdi->usb_dev, 0x40, 0x0B, 0, 0, NULL, 0, ftdi->usb_timeout) != 0) {
        ftdi->error_str = "Unable to leave bitbang mode. Perhaps not a BM type chip?";
        return -1;
    }

    ftdi->bitbang_enabled = 0;
    return 0;
}


int ftdi_read_pins(struct ftdi_context *ftdi, unsigned char *pins) {
    unsigned short usb_val;
    if (usb_control_msg(ftdi->usb_dev, 0xC0, 0x0C, 0, 0, (char *)&usb_val, 1, ftdi->usb_timeout) != 1) {
        ftdi->error_str = "Read pins failed";
        return -1;
    }

    *pins = (unsigned char)usb_val;
    return 0;
}


int ftdi_set_latency_timer(struct ftdi_context *ftdi, unsigned char latency) {
    unsigned short usb_val;

    if (latency < 1) {
       ftdi->error_str = "Latency out of range. Only valid for 1-255";
       return -1;
    }

    if (usb_control_msg(ftdi->usb_dev, 0x40, 0x09, usb_val, 0, NULL, 0, ftdi->usb_timeout) != 0) {
       ftdi->error_str = "Unable to set latency timer";
       return -2;
    }
    return 0;
}


int ftdi_get_latency_timer(struct ftdi_context *ftdi, unsigned char *latency) {
    unsigned short usb_val;
    if (usb_control_msg(ftdi->usb_dev, 0xC0, 0x09, 0, 0, (char *)&usb_val, 1, ftdi->usb_timeout) != 1) {
        ftdi->error_str = "Reading latency timer failed";
        return -1;
    }

    *latency = (unsigned char)usb_val;
    return 0;
}


int ftdi_read_eeprom(struct ftdi_context *ftdi, char *eeprom) {
    int i;

    for (i = 0; i < 64; i++) {
        if (usb_control_msg(ftdi->usb_dev, 0xC0, 0x90, 0, i, eeprom+(i*2), 2, ftdi->usb_timeout) != 2) {
           ftdi->error_str = "Reading eeprom failed";
           return -1;
        }
    }

    return 0;
}


int ftdi_write_eeprom(struct ftdi_context *ftdi, char *eeprom) {
    unsigned short usb_val;
    int i;

    for (i = 0; i < 64; i++) {
       usb_val = eeprom[i*2];
       usb_val += eeprom[(i*2)+1] << 8;
       if (usb_control_msg(ftdi->usb_dev, 0x40, 0x91, usb_val, i, NULL, 0, ftdi->usb_timeout) != 0) {
          ftdi->error_str = "Unable to write eeprom";
          return -1;
       }
    }

    return 0;
}


int ftdi_erase_eeprom(struct ftdi_context *ftdi) {
    if (usb_control_msg(ftdi->usb_dev, 0x40, 0x92, 0, 0, NULL, 0, ftdi->usb_timeout) != 0) {
        ftdi->error_str = "Unable to erase eeprom";
        return -1;
    }

    return 0;
}
