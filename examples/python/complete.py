#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""Python example program.

Complete program to demonstrate the usage
of the swig generated python wrapper

You need to build and install the wrapper first"""

import os
import ftdi
import time

# initialize
ftdic = ftdi.new()
if ftdic == 0:
  print 'new failed: %d', ret
  os._exit( 1 )

# list all devices
devlist = ftdi.new_device_listpp()
ret = ftdi.usb_find_all( ftdic, devlist, 0x0403, 0x6001 )
if ret < 0:
    print 'ftdi_usb_find_all failed: %d (%s)' % ( ret, ftdi.get_error_string( ftdic ) )
    os._exit( 1 )
print 'Number of FTDI devices found: %d\n' % ret
count = ret
curdev = devlist
for i in range( count ):
    print 'Checking device: %d' % i

    curnode = ftdi.device_listpp_value( curdev )
    ret, manufacturer, description, serial = ftdi.usb_get_strings( ftdic, curnode.dev, 128, 128, 128 )
    if ret < 0:
        print 'ftdi_usb_get_strings failed: %d (%s)' % ( ret, ftdi.get_error_string( ftdic ) )
        os._exit( 1 )
    print 'Manufacturer: %s, Description: %s, Serial: %s\n' % ( manufacturer, description, serial )
    ftdi.device_listpp_assign( curdev, curnode.next )

# open usb
ret = ftdi.usb_open( ftdic, 0x0403, 0x6001 )
if ret < 0:
    print 'unable to open ftdi device: %d (%s)' % ( ret, ftdi.get_error_string( ftdic ) )
    os._exit( 1 )


# bitbang
ret = ftdi.set_bitmode( ftdic, 0xff, ftdi.BITMODE_BITBANG )
if ret < 0:
    print 'Cannot enable bitbang'
    os._exit( 1 )
print 'turning everything on'
ftdi.write_data( ftdic, chr(0xff), 1 )
time.sleep( 1 )
print 'turning everything off\n'
ftdi.write_data( ftdic, chr(0x00), 1 )
time.sleep( 1 )
for i in range( 8 ):
    print 'enabling bit', i
    val = 2**i
    ftdi.write_data( ftdic, chr(val), 1 )
    time.sleep ( 1 )
ftdi.disable_bitbang( ftdic )
print ''


# read chip id
ret, chipid = ftdi.read_chipid( ftdic )
print 'FDTI chip id: %X\n' % chipid


# read eeprom
eeprom_addr = 1
ret, eeprom_val = ftdi.read_eeprom_location( ftdic, eeprom_addr )
print 'eeprom @ %d: 0x%04x\n' % ( eeprom_addr, eeprom_val )

print 'eeprom:'
ret=ftdi.read_eeprom( ftdic )
ret=ftdi.eeprom_decode( ftdic ,1)


# close usb
ret = ftdi.usb_close( ftdic )
if ret < 0:
    print 'unable to close ftdi device: %d (%s)' % ( ret, ftdi.get_error_string( ftdic ) )
    os._exit( 1 )
ftdi.free( ftdic )
