/* serial_read.c

   Read data via serial I/O

   This program is distributed under the GPL, version 2
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ftdi.h>

int main(int argc, char **argv)
{
    struct ftdi_context ftdic;
    char buf[1024];
    int f, i;
    int vid = 0x0403;
    int pid = 0x6001;
    int baudrate = 115200;
    int interface = INTERFACE_ANY;

    while ((i = getopt(argc, argv, "i:v:p:b:")) != -1)
    {
        switch (i)
        {
	case 'i': // 0=ANY, 1=A, 2=B, 3=C, 4=D
		interface = strtoul(optarg, NULL, 0);
		break;
	case 'v':
		vid = strtoul(optarg, NULL, 0);
		break;
	case 'p':
		pid = strtoul(optarg, NULL, 0);
		break;
	case 'b':
		baudrate = strtoul(optarg, NULL, 0);
		break;
	default:
		fprintf(stderr, "usage: %s [-i interface] [-v vid] [-p pid] [-b baudrate]\n", *argv);
		exit(-1);
        }
    }

    // Init
    if (ftdi_init(&ftdic) < 0)
    {
        fprintf(stderr, "ftdi_init failed\n");
        return EXIT_FAILURE;
    }

    // Select first interface
    ftdi_set_interface(&ftdic, interface);

    // Open device
    f = ftdi_usb_open(&ftdic, vid, pid);
    if (f < 0)
    {
        fprintf(stderr, "unable to open ftdi device: %d (%s)\n", f, ftdi_get_error_string(&ftdic));
        exit(-1);
    }

    // Set baudrate
    f = ftdi_set_baudrate(&ftdic, 115200);
    if (f < 0)
    {
        fprintf(stderr, "unable to set baudrate: %d (%s)\n", f, ftdi_get_error_string(&ftdic));
        exit(-1);
    }

    // Read data forever
    while ((f = ftdi_read_data(&ftdic, buf, sizeof(buf))) >= 0) {
	    fprintf(stderr, "read %d bytes\n", f);
	    fwrite(buf, f, 1, stdout);
	    fflush(stderr);
	    fflush(stdout);
    }

    ftdi_usb_close(&ftdic);
    ftdi_deinit(&ftdic);
}
