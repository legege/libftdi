/***************************************************************************
                          ftdi.c  -  description
                             -------------------
    begin                : Fri Apr 4 2003
    copyright            : (C) 2003 by Intra2net AG
    email                : opensource@intra2net.com
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

int ftdi_usb_purge_buffers(struct ftdi_context *ftdi) {
    if (usb_control_msg(ftdi->usb_dev, 0x40, 0, 1, 0, NULL, 0, ftdi->usb_timeout) != 0) {
        ftdi->error_str = "FTDI purge of RX buffer failed";
        return -1;
    }

    if (usb_control_msg(ftdi->usb_dev, 0x40, 0, 2, 0, NULL, 0, ftdi->usb_timeout) != 0) {
        ftdi->error_str = "FTDI purge of TX buffer failed";
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


int ftdi_write_data(struct ftdi_context *ftdi, unsigned char *buf, int size) {
    int ret;
    int offset = 0;
    while (offset < size) {
        int write_size = 64;

        if (offset+write_size > size)
            write_size = size-offset;

        ret=usb_bulk_write(ftdi->usb_dev, 2, buf+offset, write_size, ftdi->usb_timeout);
        if (ret == -1) {
    	    ftdi->error_str = "bulk write failed";
            return -1;
	}

        offset += write_size;
    }

    return 0;
}


int ftdi_read_data(struct ftdi_context *ftdi, unsigned char *buf, int size) {
    static unsigned char readbuf[64];
    int ret = 1;
    int offset = 0;

    if (size != 0 && size % 64 != 0) {
    	    ftdi->error_str = "Sorry, read buffer size must currently be a multiple (1x, 2x, 3x...) of 64";
            return -2;
    }

    while (offset < size && ret > 0) {
	ret = usb_bulk_read (ftdi->usb_dev, 0x81, readbuf, 64, ftdi->usb_timeout);
	// Skip FTDI status bytes
	if (ret >= 2)
	    ret-=2;
	
	if (ret > 0) {
	    memcpy (buf+offset, readbuf+2, ret);
	}

	if (ret == -1) {
    	    ftdi->error_str = "bulk read failed";
            return -1;
	}

        offset += ret;
    }

    return offset;
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

    usb_val = latency;
    if (usb_control_msg(ftdi->usb_dev, 0x40, 0x09, usb_val, 0, NULL, 0, ftdi->usb_timeout) != 0) {
       ftdi->error_str = "Unable to set latency timer";
       return -2;
    }
    return 0;
}


int ftdi_get_latency_timer(struct ftdi_context *ftdi, unsigned char *latency) {
    unsigned short usb_val;
    if (usb_control_msg(ftdi->usb_dev, 0xC0, 0x0A, 0, 0, (char *)&usb_val, 1, ftdi->usb_timeout) != 1) {
        ftdi->error_str = "Reading latency timer failed";
        return -1;
    }

    *latency = (unsigned char)usb_val;
    return 0;
}


void ftdi_eeprom_initdefaults(struct ftdi_eeprom *eeprom) {
    eeprom->vendor_id = 0403;
    eeprom->product_id = 6001;
    
    eeprom->self_powered = 1;
    eeprom->remote_wakeup = 1;
    eeprom->BM_type_chip = 1;
    
    eeprom->in_is_isochronous = 0;
    eeprom->out_is_isochronous = 0;
    eeprom->suspend_pull_downs = 0;
    
    eeprom->use_serial = 0;
    eeprom->change_usb_version = 0;
    eeprom->usb_version = 200;
    eeprom->max_power = 0;
    
    eeprom->manufacturer = NULL;
    eeprom->product = NULL;
    eeprom->serial = NULL;
}


/*
    ftdi_eeprom_build return codes:
    positive value: used eeprom size
    -1: eeprom size (128 bytes) exceeded by custom strings
*/
int ftdi_eeprom_build(struct ftdi_eeprom *eeprom, unsigned char *output) {
    unsigned char i, j;
    unsigned short checksum, value;
    unsigned char manufacturer_size = 0, product_size = 0, serial_size = 0;
    int size_check;

    if (eeprom->manufacturer != NULL)
	manufacturer_size = strlen(eeprom->manufacturer);
    if (eeprom->product != NULL)
	product_size = strlen(eeprom->product);
    if (eeprom->serial != NULL)
	serial_size = strlen(eeprom->serial);

    size_check = 128;	// eeprom is 128 bytes
    size_check -= 28;	// 28 are always in use (fixed)
    size_check -= manufacturer_size*2;
    size_check -= product_size*2;
    size_check -= serial_size*2;

    // eeprom size exceeded?
    if (size_check < 0)
	return (-1);

    // empty eeprom
    memset (output, 0, 128);

    // Addr 00: Stay 00 00
    // Addr 02: Vendor ID
    output[0x02] = eeprom->vendor_id;
    output[0x03] = eeprom->vendor_id >> 8;

    // Addr 04: Product ID
    output[0x04] = eeprom->product_id;
    output[0x05] = eeprom->product_id >> 8;

    // Addr 06: Device release number (0400h for BM features)
    output[0x06] = 0x00;
    
    if (eeprom->BM_type_chip == 1)
	output[0x07] = 0x04;
    else
	output[0x07] = 0x02;

    // Addr 08: Config descriptor
    // Bit 1: remote wakeup if 1
    // Bit 0: self powered if 1
    //
    j = 0;
    if (eeprom->self_powered == 1)
	j = j | 1;
    if (eeprom->remote_wakeup == 1)
	j = j | 2;
    output[0x08] = j;

    // Addr 09: Max power consumption: max power = value * 2 mA
    output[0x09] = eeprom->max_power;;
    
    // Addr 0A: Chip configuration
    // Bit 7: 0 - reserved
    // Bit 6: 0 - reserved
    // Bit 5: 0 - reserved
    // Bit 4: 1 - Change USB version
    // Bit 3: 1 - Use the serial number string
    // Bit 2: 1 - Enable suspend pull downs for lower power
    // Bit 1: 1 - Out EndPoint is Isochronous
    // Bit 0: 1 - In EndPoint is Isochronous
    //
    j = 0;
    if (eeprom->in_is_isochronous == 1)
	j = j | 1;
    if (eeprom->out_is_isochronous == 1)
	j = j | 2;
    if (eeprom->suspend_pull_downs == 1)
	j = j | 4;
    if (eeprom->use_serial == 1)
	j = j | 8;
    if (eeprom->change_usb_version == 1)
	j = j | 16;
    output[0x0A] = j;
    
    // Addr 0B: reserved
    output[0x0B] = 0x00;
    
    // Addr 0C: USB version low byte when 0x0A bit 4 is set
    // Addr 0D: USB version high byte when 0x0A bit 4 is set
    if (eeprom->change_usb_version == 1) {
        output[0x0C] = eeprom->usb_version;
	output[0x0D] = eeprom->usb_version >> 8;
    }


    // Addr 0E: Offset of the manufacturer string + 0x80
    output[0x0E] = 0x14 + 0x80;

    // Addr 0F: Length of manufacturer string
    output[0x0F] = manufacturer_size*2 + 2;

    // Addr 10: Offset of the product string + 0x80, calculated later
    // Addr 11: Length of product string
    output[0x11] = product_size*2 + 2;

    // Addr 12: Offset of the serial string + 0x80, calculated later
    // Addr 13: Length of serial string
    output[0x13] = serial_size*2 + 2;

    // Dynamic content
    output[0x14] = manufacturer_size*2 + 2;
    output[0x15] = 0x03;	// type: string
    
    i = 0x16, j = 0;
    
    // Output manufacturer
    for (j = 0; j < manufacturer_size; j++) {
	output[i] = eeprom->manufacturer[j], i++;
	output[i] = 0x00, i++;
    }

    // Output product name
    output[0x10] = i + 0x80; 	// calculate offset
    output[i] = product_size*2 + 2, i++;
    output[i] = 0x03, i++;
    for (j = 0; j < product_size; j++) {
	output[i] = eeprom->product[j], i++;
	output[i] = 0x00, i++;
    }
    
    // Output serial
    output[0x12] = i + 0x80;	// calculate offset
    output[i] = serial_size*2 + 2, i++;
    output[i] = 0x03, i++;
    for (j = 0; j < serial_size; j++) {
	output[i] = eeprom->serial[j], i++;
	output[i] = 0x00, i++;
    }

    // calculate checksum
    checksum = 0xAAAA;
    
    for (i = 0; i < 63; i++) {
	value = output[i*2];
	value += output[(i*2)+1] << 8;

	checksum = value^checksum;
	checksum = (checksum << 1) | (checksum >> 15);	
    }

    output[0x7E] = checksum;
    output[0x7F] = checksum >> 8;    

    return size_check;
}


int ftdi_read_eeprom(struct ftdi_context *ftdi, unsigned char *eeprom) {
    int i;

    for (i = 0; i < 64; i++) {
        if (usb_control_msg(ftdi->usb_dev, 0xC0, 0x90, 0, i, eeprom+(i*2), 2, ftdi->usb_timeout) != 2) {
           ftdi->error_str = "Reading eeprom failed";
           return -1;
        }
    }

    return 0;
}


int ftdi_write_eeprom(struct ftdi_context *ftdi, unsigned char *eeprom) {
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
