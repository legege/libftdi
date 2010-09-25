/* LIBFTDI EEPROM access example

   This program is distributed under the GPL, version 2
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>
#include <ftdi.h>

int main(int argc, char **argv)
{
    struct ftdi_context *ftdi;
    unsigned char *buf;
    int f, i, j;
    int vid = 0x0403;
    int pid = 0x6010;
    char const *desc    = 0;
    char const *serial  = 0;
    int erase = 0;
    int use_defaults = 0;
    int large_chip = 0;
    int do_write = 0;
    int size;

    if ((ftdi = ftdi_new()) == 0)
    {
       fprintf(stderr, "Failed to allocate ftdi structure :%s \n", 
		   ftdi_get_error_string(ftdi));
        return EXIT_FAILURE;
    }

    while ((i = getopt(argc, argv, "d::ev:p:l:P:S:w")) != -1)
    {
        switch (i)
        {
        case 'd':
            use_defaults = 1;
            if(optarg)
                large_chip = 0x66; 
            break;
        case 'e':
            erase = 1;
            break;
        case 'v':
		vid = strtoul(optarg, NULL, 0);
		break;
	case 'p':
		pid = strtoul(optarg, NULL, 0);
		break;
	case 'P':
                desc = optarg;
		break;
	case 'S':
		serial = optarg;
		break;
	case 'w':
		do_write  = 1;
		break;
	default:
		fprintf(stderr, "usage: %s [options]\n", *argv);
		fprintf(stderr, "\t-d[num] Work with default valuesfor 128 Byte "
                        "EEPROM or for 256 Byte EEPROm if some [num] is given\n");
		fprintf(stderr, "\t-w write\n");
		fprintf(stderr, "\t-e erase\n");
		fprintf(stderr, "\t-v verbose decoding\n");
		fprintf(stderr, "\t-p <number> Search for device with PID == number\n");
		fprintf(stderr, "\t-v <number> Search for device with VID == number\n");
		fprintf(stderr, "\t-P <string? Search for device with given "
                        "product description\n");
		fprintf(stderr, "\t-S <string? Search for device with given "
                        "serial number\n");
		exit(-1);
        }
    }

    // Select first interface
    ftdi_set_interface(ftdi, INTERFACE_ANY);

    // Open device
    f = ftdi_usb_open_desc(ftdi, vid, pid, desc, serial);
    if (f < 0)
    {
        fprintf(stderr, "Device VID 0x%04x PID 0x%04x", vid, pid);
        if(desc)
            fprintf(stderr, " Desc %s", desc);
        if(serial)
            fprintf(stderr, " Serial %s", serial);
        fprintf(stderr, "\n");
        fprintf(stderr, "unable to open ftdi device: %d (%s)\n", 
		f, ftdi_get_error_string(ftdi));

        exit(-1);
    }

    if (erase)
    {
        f = ftdi_erase_eeprom(ftdi);
        if (f < 0)
        {
            fprintf(stderr, "Erase failed: %s", 
                    ftdi_get_error_string(ftdi));
            return -2;
        }
        if (ftdi->eeprom->chip == -1)
            fprintf(stderr, "No EEPROM\n");
        else if (ftdi->eeprom->chip == 0)
            fprintf(stderr, "Internal EEPROM\n");
        else
            fprintf(stderr, "Found 93x%02x\n",ftdi->eeprom->chip);
        return 0;
    }        

    if(use_defaults)
    {
        ftdi_eeprom_initdefaults(ftdi, "IKDA", "FTDIJTAF", "0001");
        ftdi->eeprom->max_power = 500;
        f=(ftdi_eeprom_build(ftdi));
        if (f < 0)
        {
            fprintf(stderr, "ftdi_eeprom_build: %d (%s)\n", 
                    f, ftdi_get_error_string(ftdi));
            exit(-1);
        }
    }
    else if(do_write)
    {
        ftdi_eeprom_initdefaults(ftdi, "IKDA", "FTDIJTAG", "0001");
        ftdi->eeprom->max_power = 500;
        f = ftdi_erase_eeprom(ftdi);
        if (ftdi->eeprom->chip == -1)
            fprintf(stderr, "No EEPROM\n");
        else if (ftdi->eeprom->chip == 0)
            fprintf(stderr, "Internal EEPROM\n");
        else
            fprintf(stderr, "Found 93x%02x\n",ftdi->eeprom->chip);
        f=(ftdi_eeprom_build(ftdi));
        if (f < 0)
        {
            fprintf(stderr, "Erase failed: %s", 
                    ftdi_get_error_string(ftdi));
            return -2;
        }
        f = ftdi_write_eeprom(ftdi);
        {
            fprintf(stderr, "ftdi_eeprom_decode: %d (%s)\n", 
                    f, ftdi_get_error_string(ftdi));
            exit(-1);
        }
        f = ftdi_read_eeprom(ftdi);
        if (f < 0)
        {
            fprintf(stderr, "ftdi_read_eeprom: %d (%s)\n", 
                    f, ftdi_get_error_string(ftdi));
            exit(-1);
        }
     }
    else
    {
        f = ftdi_read_eeprom(ftdi);
        if (f < 0)
        {
            fprintf(stderr, "ftdi_read_eeprom: %d (%s)\n", 
                    f, ftdi_get_error_string(ftdi));
            exit(-1);
        }
    }


    fprintf(stderr, "Chip type %d ftdi_eeprom_size: %d\n", ftdi->type, ftdi->eeprom->size);
    buf = ftdi->eeprom->buf;
    if (ftdi->type == TYPE_R)
        size = 0xa0;
    else
        size = ftdi->eeprom->size;
    for(i=0; i < size; i += 16)
    {
	fprintf(stdout,"0x%03x:", i);
	
	for (j = 0; j< 8; j++)
	    fprintf(stdout," %02x", buf[i+j]);
	fprintf(stdout," ");
	for (; j< 16; j++)
	    fprintf(stdout," %02x", buf[i+j]);
	fprintf(stdout," ");
	for (j = 0; j< 8; j++)
	    fprintf(stdout,"%c", isprint(buf[i+j])?buf[i+j]:'.');
	fprintf(stdout," ");
	for (; j< 16; j++)
	    fprintf(stdout,"%c", isprint(buf[i+j])?buf[i+j]:'.');
	fprintf(stdout,"\n");
    }

    f = ftdi_eeprom_decode(ftdi, 1);
    if (f < 0)
    {
        fprintf(stderr, "ftdi_eeprom_decode: %d (%s)\n", 
		f, ftdi_get_error_string(ftdi));
        exit(-1);
    }

    ftdi_usb_close(ftdi);
    ftdi_free(ftdi);
    return 0;
}
