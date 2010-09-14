/***************************************************************************
                          ftdi.h  -  description
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

#ifndef __libftdi_h__
#define __libftdi_h__

#include <libusb.h>

/* Evne on 93xx66 at max 256 bytes are used (AN_121)*/
#define FTDI_MAX_EEPROM_SIZE 256

/** FTDI chip type */
enum ftdi_chip_type { TYPE_AM=0, TYPE_BM=1, TYPE_2232C=2, TYPE_R=3, TYPE_2232H=4, TYPE_4232H=5 };
/** Parity mode for ftdi_set_line_property() */
enum ftdi_parity_type { NONE=0, ODD=1, EVEN=2, MARK=3, SPACE=4 };
/** Number of stop bits for ftdi_set_line_property() */
enum ftdi_stopbits_type { STOP_BIT_1=0, STOP_BIT_15=1, STOP_BIT_2=2 };
/** Number of bits for ftdi_set_line_property() */
enum ftdi_bits_type { BITS_7=7, BITS_8=8 };
/** Break type for ftdi_set_line_property2() */
enum ftdi_break_type { BREAK_OFF=0, BREAK_ON=1 };

/** MPSSE bitbang modes */
enum ftdi_mpsse_mode
{
    BITMODE_RESET  = 0x00,    /**< switch off bitbang mode, back to regular serial/FIFO */
    BITMODE_BITBANG= 0x01,    /**< classical asynchronous bitbang mode, introduced with B-type chips */
    BITMODE_MPSSE  = 0x02,    /**< MPSSE mode, available on 2232x chips */
    BITMODE_SYNCBB = 0x04,    /**< synchronous bitbang mode, available on 2232x and R-type chips  */
    BITMODE_MCU    = 0x08,    /**< MCU Host Bus Emulation mode, available on 2232x chips */
                              /* CPU-style fifo mode gets set via EEPROM */
    BITMODE_OPTO   = 0x10,    /**< Fast Opto-Isolated Serial Interface Mode, available on 2232x chips  */
    BITMODE_CBUS   = 0x20,    /**< Bitbang on CBUS pins of R-type chips, configure in EEPROM before */
    BITMODE_SYNCFF = 0x40,    /**< Single Channel Synchronous FIFO mode, available on 2232H chips */
};

/** Port interface for chips with multiple interfaces */
enum ftdi_interface
{
    INTERFACE_ANY = 0,
    INTERFACE_A   = 1,
    INTERFACE_B   = 2,
    INTERFACE_C   = 3,
    INTERFACE_D   = 4
};

/* Shifting commands IN MPSSE Mode*/
#define MPSSE_WRITE_NEG 0x01   /* Write TDI/DO on negative TCK/SK edge*/
#define MPSSE_BITMODE   0x02   /* Write bits, not bytes */
#define MPSSE_READ_NEG  0x04   /* Sample TDO/DI on negative TCK/SK edge */
#define MPSSE_LSB       0x08   /* LSB first */
#define MPSSE_DO_WRITE  0x10   /* Write TDI/DO */
#define MPSSE_DO_READ   0x20   /* Read TDO/DI */
#define MPSSE_WRITE_TMS 0x40   /* Write TMS/CS */

/* FTDI MPSSE commands */
#define SET_BITS_LOW   0x80
/*BYTE DATA*/
/*BYTE Direction*/
#define SET_BITS_HIGH  0x82
/*BYTE DATA*/
/*BYTE Direction*/
#define GET_BITS_LOW   0x81
#define GET_BITS_HIGH  0x83
#define LOOPBACK_START 0x84
#define LOOPBACK_END   0x85
#define TCK_DIVISOR    0x86
/* Value Low */
/* Value HIGH */ /*rate is 12000000/((1+value)*2) */
#define DIV_VALUE(rate) (rate > 6000000)?0:((6000000/rate -1) > 0xffff)? 0xffff: (6000000/rate -1)

/* Commands in MPSSE and Host Emulation Mode */
#define SEND_IMMEDIATE 0x87
#define WAIT_ON_HIGH   0x88
#define WAIT_ON_LOW    0x89

/* Commands in Host Emulation Mode */
#define READ_SHORT     0x90
/* Address_Low */
#define READ_EXTENDED  0x91
/* Address High */
/* Address Low  */
#define WRITE_SHORT    0x92
/* Address_Low */
#define WRITE_EXTENDED 0x93
/* Address High */
/* Address Low  */

/* Definitions for flow control */
#define SIO_RESET          0 /* Reset the port */
#define SIO_MODEM_CTRL     1 /* Set the modem control register */
#define SIO_SET_FLOW_CTRL  2 /* Set flow control register */
#define SIO_SET_BAUD_RATE  3 /* Set baud rate */
#define SIO_SET_DATA       4 /* Set the data characteristics of the port */

#define FTDI_DEVICE_OUT_REQTYPE (LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_OUT)
#define FTDI_DEVICE_IN_REQTYPE (LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_IN)

/* Requests */
#define SIO_RESET_REQUEST             SIO_RESET
#define SIO_SET_BAUDRATE_REQUEST      SIO_SET_BAUD_RATE
#define SIO_SET_DATA_REQUEST          SIO_SET_DATA
#define SIO_SET_FLOW_CTRL_REQUEST     SIO_SET_FLOW_CTRL
#define SIO_SET_MODEM_CTRL_REQUEST    SIO_MODEM_CTRL
#define SIO_POLL_MODEM_STATUS_REQUEST 0x05
#define SIO_SET_EVENT_CHAR_REQUEST    0x06
#define SIO_SET_ERROR_CHAR_REQUEST    0x07
#define SIO_SET_LATENCY_TIMER_REQUEST 0x09
#define SIO_GET_LATENCY_TIMER_REQUEST 0x0A
#define SIO_SET_BITMODE_REQUEST       0x0B
#define SIO_READ_PINS_REQUEST         0x0C
#define SIO_READ_EEPROM_REQUEST       0x90
#define SIO_WRITE_EEPROM_REQUEST      0x91
#define SIO_ERASE_EEPROM_REQUEST      0x92


#define SIO_RESET_SIO 0
#define SIO_RESET_PURGE_RX 1
#define SIO_RESET_PURGE_TX 2

#define SIO_DISABLE_FLOW_CTRL 0x0
#define SIO_RTS_CTS_HS (0x1 << 8)
#define SIO_DTR_DSR_HS (0x2 << 8)
#define SIO_XON_XOFF_HS (0x4 << 8)

#define SIO_SET_DTR_MASK 0x1
#define SIO_SET_DTR_HIGH ( 1 | ( SIO_SET_DTR_MASK  << 8))
#define SIO_SET_DTR_LOW  ( 0 | ( SIO_SET_DTR_MASK  << 8))
#define SIO_SET_RTS_MASK 0x2
#define SIO_SET_RTS_HIGH ( 2 | ( SIO_SET_RTS_MASK << 8 ))
#define SIO_SET_RTS_LOW ( 0 | ( SIO_SET_RTS_MASK << 8 ))

#define SIO_RTS_CTS_HS (0x1 << 8)

/* marker for unused usb urb structures
   (taken from libusb) */
#define FTDI_URB_USERCONTEXT_COOKIE ((void *)0x1)

#ifdef __GNUC__
    #define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
    #define DEPRECATED(func) __declspec(deprecated) func
#else
    #pragma message("WARNING: You need to implement DEPRECATED for this compiler")
    #define DEPRECATED(func) func
#endif

struct ftdi_transfer_control
{
    int completed;
    unsigned char *buf;
    int size;
    int offset;
    struct ftdi_context *ftdi;
    struct libusb_transfer *transfer;
};

/**
    \brief FTDI eeprom structure
*/
struct ftdi_eeprom
{
    /** vendor id */
    int vendor_id;
    /** product id */
    int product_id;

    /** self powered */
    int self_powered;
    /** remote wakeup */
    int remote_wakeup;

    /* Suspend on DBUS7 Low */
    int suspend_dbus7;

    /** input in isochronous transfer mode */
    int in_is_isochronous;
    /** output in isochronous transfer mode */
    int out_is_isochronous;
    /** suspend pull downs */
    int suspend_pull_downs;

    /** use serial */
    int use_serial;
    /** usb version */
    int usb_version;
    /** maximum power */
    int max_power;

    /** manufacturer name */
    char *manufacturer;
    /** product name */
    char *product;
    /** serial number */
    char *serial;

    /* 2232D/H(/FT4432H?) specific */
    /* Hardware type, 0 = RS232 Uart, 1 = 245 FIFO, 2 = CPU FIFO, 
       4 = OPTO Isolate */
    int channel_a_type;
    int channel_b_type;
    /*  Driver Type, 1 = VCP */
    int channel_a_driver;
    int channel_b_driver;

    /* Special function of FT232R devices (and possibly others as well) */
    /** CBUS pin function. See CBUS_xxx defines. */
    int cbus_function[5];
    /** Select hight current drive on R devices. */
    int high_current;
    /** Select hight current drive on A channel (2232C */
    int high_current_a;
    /** Select hight current drive on B channel (2232C). */
    int high_current_b;
    /** Select inversion of data lines (bitmask). */
    int invert;

    /*2232H/4432H Group specific values */
    /* Group0 is AL on 2322H and A on 4232H
       Group1 is AH on 2232H and B on 4232H
       Group2 is BL on 2322H and C on 4232H
       Group3 is BH on 2232H and C on 4232H*/
    int group0_drive;
    int group0_schmitt;
    int group0_slew;
    int group1_drive;
    int group1_schmitt;
    int group1_slew;
    int group2_drive;
    int group2_schmitt;
    int group2_slew;
    int group3_drive;
    int group3_schmitt;
    int group3_slew;
    

    /** eeprom size in bytes. This doesn't get stored in the eeprom
        but is the only way to pass it to ftdi_eeprom_build. */
    int size;
    /* EEPROM Type 46 for 93xx46, 56 for 93xx56 and 66 for 93xx66*/
    int chip;
    unsigned char buf[FTDI_MAX_EEPROM_SIZE];
};

/**
    \brief Main context structure for all libftdi functions.

    Do not access directly if possible.
*/
struct ftdi_context
{
    /* USB specific */
    /** libusb's context */
    struct libusb_context *usb_ctx;
    /** libusb's usb_dev_handle */
    struct libusb_device_handle *usb_dev;
    /** usb read timeout */
    int usb_read_timeout;
    /** usb write timeout */
    int usb_write_timeout;

    /* FTDI specific */
    /** FTDI chip type */
    enum ftdi_chip_type type;
    /** baudrate */
    int baudrate;
    /** bitbang mode state */
    unsigned char bitbang_enabled;
    /** pointer to read buffer for ftdi_read_data */
    unsigned char *readbuffer;
    /** read buffer offset */
    unsigned int readbuffer_offset;
    /** number of remaining data in internal read buffer */
    unsigned int readbuffer_remaining;
    /** read buffer chunk size */
    unsigned int readbuffer_chunksize;
    /** write buffer chunk size */
    unsigned int writebuffer_chunksize;
    /** maximum packet size. Needed for filtering modem status bytes every n packets. */
    unsigned int max_packet_size;

    /* FTDI FT2232C requirecments */
    /** FT2232C interface number: 0 or 1 */
    int interface;   /* 0 or 1 */
    /** FT2232C index number: 1 or 2 */
    int index;       /* 1 or 2 */
    /* Endpoints */
    /** FT2232C end points: 1 or 2 */
    int in_ep;
    int out_ep;      /* 1 or 2 */

    /** Bitbang mode. 1: (default) Normal bitbang mode, 2: FT2232C SPI bitbang mode */
    unsigned char bitbang_mode;

    /** Decoded eeprom structure */
    struct ftdi_eeprom *eeprom;

    /** String representation of last error */
    char *error_str;
};

/**
    \brief list of usb devices created by ftdi_usb_find_all()
*/
struct ftdi_device_list
{
    /** pointer to next entry */
    struct ftdi_device_list *next;
    /** pointer to libusb's usb_device */
    struct libusb_device *dev;
};

#define USE_SERIAL_NUM 0x08
enum ftdi_cbus_func {/* FIXME: Recheck value, especially the last */
    CBUS_TXDEN = 0, CBUS_PWREN = 1, CBUS_RXLED = 2, CBUS_TXLED = 3, CBUS_TXRXLED = 4,
    CBUS_SLEEP = 5, CBUS_CLK48 = 6, CBUS_CLK24 = 7, CBUS_CLK12 = 8, CBUS_CLK6 =  9,
    CBUS_IOMODE = 0xa, CBUS_BB_WR = 0xb, CBUS_BB_RD = 0xc, CBUS_BB   = 0xd};

/** Invert TXD# */
#define INVERT_TXD 0x01
/** Invert RXD# */
#define INVERT_RXD 0x02
/** Invert RTS# */
#define INVERT_RTS 0x04
/** Invert CTS# */
#define INVERT_CTS 0x08
/** Invert DTR# */
#define INVERT_DTR 0x10
/** Invert DSR# */
#define INVERT_DSR 0x20
/** Invert DCD# */
#define INVERT_DCD 0x40
/** Invert RI# */
#define INVERT_RI  0x80

/** Interface Mode. */
#define CHANNEL_IS_UART 0x0
#define CHANNEL_IS_245  0x1
#define CHANNEL_IS_CPU  0x2
#define CHANNEL_IS_OPTO 0x4

#define DRIVE_4MA  0
#define DRIVE_8MA  1
#define DRIVE_12MA 2
#define DRIVE_16MA 3
#define SLOW_SLEW  4
#define IS_SCHMITT 8

/** Driver Type. */
#define DRIVER_VCP 0x08

#define SUSPEND_DBUS7 0x80

/** High current drive. */
#define HIGH_CURRENT_DRIVE   0x10
#define HIGH_CURRENT_DRIVE_R 0x04

/**
    \brief Progress Info for streaming read
*/
struct size_and_time
{
        uint64_t totalBytes;
        struct timeval time;
};

typedef struct
{
    struct size_and_time first;
    struct size_and_time prev;
    struct size_and_time current;
    double totalTime;
    double totalRate;
    double currentRate;
} FTDIProgressInfo;

typedef int (FTDIStreamCallback)(uint8_t *buffer, int length,
                                 FTDIProgressInfo *progress, void *userdata);


#ifdef __cplusplus
extern "C"
{
#endif

    int ftdi_init(struct ftdi_context *ftdi);
    struct ftdi_context *ftdi_new(void);
    int ftdi_set_interface(struct ftdi_context *ftdi, enum ftdi_interface interface);

    void ftdi_deinit(struct ftdi_context *ftdi);
    void ftdi_free(struct ftdi_context *ftdi);
    void ftdi_set_usbdev (struct ftdi_context *ftdi, struct libusb_device_handle *usbdev);

    int ftdi_usb_find_all(struct ftdi_context *ftdi, struct ftdi_device_list **devlist,
                          int vendor, int product);
    void ftdi_list_free(struct ftdi_device_list **devlist);
    void ftdi_list_free2(struct ftdi_device_list *devlist);
    int ftdi_usb_get_strings(struct ftdi_context *ftdi, struct libusb_device *dev,
                             char * manufacturer, int mnf_len,
                             char * description, int desc_len,
                             char * serial, int serial_len);

    int ftdi_usb_open(struct ftdi_context *ftdi, int vendor, int product);
    int ftdi_usb_open_desc(struct ftdi_context *ftdi, int vendor, int product,
                           const char* description, const char* serial);
    int ftdi_usb_open_desc_index(struct ftdi_context *ftdi, int vendor, int product,
                           const char* description, const char* serial, unsigned int index);
    int ftdi_usb_open_dev(struct ftdi_context *ftdi, struct libusb_device *dev);
    int ftdi_usb_open_string(struct ftdi_context *ftdi, const char* description);

    int ftdi_usb_close(struct ftdi_context *ftdi);
    int ftdi_usb_reset(struct ftdi_context *ftdi);
    int ftdi_usb_purge_rx_buffer(struct ftdi_context *ftdi);
    int ftdi_usb_purge_tx_buffer(struct ftdi_context *ftdi);
    int ftdi_usb_purge_buffers(struct ftdi_context *ftdi);

    int ftdi_set_baudrate(struct ftdi_context *ftdi, int baudrate);
    int ftdi_set_line_property(struct ftdi_context *ftdi, enum ftdi_bits_type bits,
                               enum ftdi_stopbits_type sbit, enum ftdi_parity_type parity);
    int ftdi_set_line_property2(struct ftdi_context *ftdi, enum ftdi_bits_type bits,
                                enum ftdi_stopbits_type sbit, enum ftdi_parity_type parity,
                                enum ftdi_break_type break_type);

    int ftdi_read_data(struct ftdi_context *ftdi, unsigned char *buf, int size);
    int ftdi_read_data_set_chunksize(struct ftdi_context *ftdi, unsigned int chunksize);
    int ftdi_read_data_get_chunksize(struct ftdi_context *ftdi, unsigned int *chunksize);

    int ftdi_write_data(struct ftdi_context *ftdi, unsigned char *buf, int size);
    int ftdi_write_data_set_chunksize(struct ftdi_context *ftdi, unsigned int chunksize);
    int ftdi_write_data_get_chunksize(struct ftdi_context *ftdi, unsigned int *chunksize);

    int ftdi_readstream(struct ftdi_context *ftdi, FTDIStreamCallback *callback, 
                        void *userdata, int packetsPerTransfer, int numTransfers);
    int ftdi_write_data_async(struct ftdi_context *ftdi, unsigned char *buf, int size);
    void ftdi_async_complete(struct ftdi_context *ftdi, int wait_for_more);

    struct ftdi_transfer_control *ftdi_read_data_submit(struct ftdi_context *ftdi, unsigned char *buf, int size);
    int ftdi_transfer_data_done(struct ftdi_transfer_control *tc);

    int DEPRECATED(ftdi_enable_bitbang(struct ftdi_context *ftdi, unsigned char bitmask));
    int ftdi_disable_bitbang(struct ftdi_context *ftdi);
    int ftdi_set_bitmode(struct ftdi_context *ftdi, unsigned char bitmask, unsigned char mode);
    int ftdi_read_pins(struct ftdi_context *ftdi, unsigned char *pins);

    int ftdi_set_latency_timer(struct ftdi_context *ftdi, unsigned char latency);
    int ftdi_get_latency_timer(struct ftdi_context *ftdi, unsigned char *latency);

    int ftdi_poll_modem_status(struct ftdi_context *ftdi, unsigned short *status);

    /* flow control */
    int ftdi_setflowctrl(struct ftdi_context *ftdi, int flowctrl);
    int ftdi_setdtr_rts(struct ftdi_context *ftdi, int dtr, int rts);
    int ftdi_setdtr(struct ftdi_context *ftdi, int state);
    int ftdi_setrts(struct ftdi_context *ftdi, int state);

    int ftdi_set_event_char(struct ftdi_context *ftdi, unsigned char eventch, unsigned char enable);
    int ftdi_set_error_char(struct ftdi_context *ftdi, unsigned char errorch, unsigned char enable);

    /* init and build eeprom from ftdi_eeprom structure */
    void ftdi_eeprom_initdefaults(struct ftdi_context *ftdi);
    void ftdi_eeprom_free(struct ftdi_context *ftdi);
    int ftdi_eeprom_build(struct ftdi_context *ftdi);
    int ftdi_eeprom_decode(struct ftdi_context *ftdi, int verbose);

    int ftdi_read_eeprom(struct ftdi_context *ftdi);
    int ftdi_read_chipid(struct ftdi_context *ftdi, unsigned int *chipid);
    int ftdi_write_eeprom(struct ftdi_context *ftdi);
    int ftdi_erase_eeprom(struct ftdi_context *ftdi);

    int ftdi_read_eeprom_location (struct ftdi_context *ftdi, int eeprom_addr, unsigned short *eeprom_val);
    int ftdi_write_eeprom_location(struct ftdi_context *ftdi, int eeprom_addr, unsigned short eeprom_val);

    char *ftdi_get_error_string(struct ftdi_context *ftdi);

#ifdef __cplusplus
}
#endif

#endif /* __libftdi_h__ */
