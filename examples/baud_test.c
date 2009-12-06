/* baud_test.c
 *
 * test setting the baudrate and compare it with the expected runtime
 *
 * options:
 *  -p <usb-product-id> (vendor is fixed to ftdi / 0x0403)
 *  -d <datasize to send in bytes>
 *  -b <baudrate> (multiplies by 16 if bitbang as written in the ftdi datasheets)
 *  -m <mode to use> r: serial a: async bitbang s:sync bitbang 
 *
 * (C) 2009 by Gerd v. Egidy <gerd.von.egidy@intra2net.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <ftdi.h>

double get_prec_time()
{
    struct timeval tv;
    double res;
    
    gettimeofday(&tv,NULL);

    res=tv.tv_sec;
    res+=((double)tv.tv_usec/1000000);

    return res;
}

int main(int argc, char **argv)
{
    struct ftdi_context ftdic;
    int i, t;
    char txbuf[256];
    char rxbuf[4096];
    double start, duration, plan;

    // default values
    int baud=9600;
    int set_baud;
    int datasize=100000;
    int product_id=0x6001;
    enum ftdi_mpsse_mode test_mode=BITMODE_BITBANG;
    
    while ((t = getopt (argc, argv, "b:d:p:m:")) != -1)
    {
        switch (t)
        {
            case 'd':
                datasize = atoi (optarg);
                break;
            case 'm':
                switch(*optarg)
                {
                    case 'r':
                        // serial
                        test_mode=BITMODE_RESET;
                        break;
                    case 'a':
                        // async
                        test_mode=BITMODE_BITBANG;
                        break;
                    case 's':
                        // sync
                        test_mode=BITMODE_SYNCBB;
                        break;
                }
                break;
            case 'b':
                baud = atoi (optarg);
                break;
            case 'p':
                sscanf(optarg,"0x%x",&product_id);
                break;
        }
    }

    if (ftdi_init(&ftdic) < 0)
    {
        fprintf(stderr, "ftdi_init failed\n");
        return EXIT_FAILURE;
    }

    if (ftdi_usb_open(&ftdic, 0x0403, product_id) < 0)
    {
        fprintf(stderr,"Can't open ftdi device: %s\n",ftdi_get_error_string(&ftdic));
        return EXIT_FAILURE;
    }

    set_baud=baud;
    if (test_mode!=BITMODE_RESET)
    {
        // we do bitbang, so real baudrate / 16
        set_baud=baud/16;
    }

    ftdi_set_baudrate(&ftdic,set_baud);
    printf("real baudrate used: %d\n",(test_mode==BITMODE_RESET) ? ftdic.baudrate : ftdic.baudrate*16);

    if (ftdi_set_bitmode(&ftdic, 0xFF,test_mode) < 0)
    {
        fprintf(stderr,"Can't set mode: %s\n",ftdi_get_error_string(&ftdic));
        return EXIT_FAILURE;
    }

    if (test_mode==BITMODE_RESET)
    {
        // serial 8N1: 8 data bits, 1 startbit, 1 stopbit
        plan=((double)(datasize*10))/baud;
    }
    else
    {
        // bitbang means 8 bits at once
        plan=((double)datasize)/baud;
    }

    printf("this test should take %.2f seconds\n",plan);

    // prepare data to send: 0 and 1 bits alternating (except for serial start/stopbit):
    // maybe someone wants to look at this with a scope or logic analyzer
    for (i=0; i<sizeof(txbuf); i++)
    {
        if (test_mode==BITMODE_RESET)
            txbuf[i]=0xAA;
        else
            txbuf[i]=(i%2) ? 0xff : 0;
    }

    if(test_mode==BITMODE_SYNCBB)
    {
        // clear the receive buffer before beginning
        // will read only as much as available
        ftdi_read_data(&ftdic, rxbuf, sizeof(rxbuf));
    }
    
    start=get_prec_time();

    i=0;
    while(i < datasize)
    {
        int sendsize=sizeof(txbuf);
        if (i+sendsize > datasize)
            sendsize=datasize-i;

        if ((sendsize=ftdi_write_data(&ftdic, txbuf, sendsize)) < 0)
        {
            fprintf(stderr,"write failed at %d: %s\n",
                    i, ftdi_get_error_string(&ftdic));
            return EXIT_FAILURE;
        }

        i+=sendsize;

        if(test_mode==BITMODE_SYNCBB)
        {
            // read everything available
            ftdi_read_data(&ftdic, rxbuf, sizeof(rxbuf));
        }
    }

    duration=get_prec_time()-start;
    printf("and took %.4f seconds, this is factor %.2f\n",duration,plan/duration);

    ftdi_usb_close(&ftdic);
    ftdi_deinit(&ftdic);
    exit (0);
}
