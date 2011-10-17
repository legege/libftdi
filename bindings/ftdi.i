/* File: ftdi.i */

%module(docstring="Python interface to libftdi") ftdi

%include "typemaps.i"
%include "cpointer.i"
%include "cstring.i"

%typemap(in) unsigned char* = char*;

%ignore ftdi_write_data_async;
%ignore ftdi_async_complete;

%immutable ftdi_version_info::version_str;
%immutable ftdi_version_info::snapshot_str;

%rename("%(strip:[ftdi_])s") "";

%apply char *OUTPUT { char * manufacturer };
%apply char *OUTPUT { char * description };
%apply char *OUTPUT { char * serial };
%cstring_bounded_output( char * manufacturer, 256 );
%cstring_bounded_output( char * description, 256 );
%cstring_bounded_output( char * serial, 256 );
    int ftdi_usb_get_strings(struct ftdi_context *ftdi, struct libusb_device *dev,
                             char * manufacturer, int mnf_len,
                             char * description, int desc_len,
                             char * serial, int serial_len);
%clear char * manufacturer;
%clear char * description;
%clear char * serial;

%apply char *OUTPUT { unsigned char *buf };
    int ftdi_read_data(struct ftdi_context *ftdi, unsigned char *buf, int size);
%clear unsigned char *buf;

%apply int *OUTPUT { unsigned int *chunksize };
    int ftdi_read_data_get_chunksize(struct ftdi_context *ftdi, unsigned int *chunksize);
    int ftdi_write_data_get_chunksize(struct ftdi_context *ftdi, unsigned int *chunksize);
%clear unsigned int *chunksize;

%apply char *OUTPUT { unsigned char *pins };
    int ftdi_read_pins(struct ftdi_context *ftdi, unsigned char *pins);
%clear unsigned char *pins;

%apply char *OUTPUT { unsigned char *latency };
    int ftdi_get_latency_timer(struct ftdi_context *ftdi, unsigned char *latency);
%clear unsigned char *latency;

%apply short *OUTPUT { unsigned short *status };
    int ftdi_poll_modem_status(struct ftdi_context *ftdi, unsigned short *status);
%clear unsigned short *status;

%apply int *OUTPUT { int* value };
    int ftdi_get_eeprom_value(struct ftdi_context *ftdi, enum ftdi_eeprom_value value_name, int* value);
%clear int* value;

%apply short *OUTPUT { unsigned short *eeprom_val };
    int ftdi_read_eeprom_location (struct ftdi_context *ftdi, int eeprom_addr, unsigned short *eeprom_val);
%clear unsigned short *eeprom_val;

%apply int *OUTPUT { unsigned int *chipid };
    int ftdi_read_chipid(struct ftdi_context *ftdi, unsigned int *chipid);
%clear unsigned int *chipid;

%include ftdi.h
%{
#include <ftdi.h>
%}

%include ftdi_i.h
%{
#include <ftdi_i.h>
%}

%pointer_functions(struct ftdi_device_list *, device_listpp)
