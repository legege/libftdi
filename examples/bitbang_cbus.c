/*  bitbang_cbus.c

    Example to use CBUS bitbang mode of newer chipsets.
    You must enable CBUS bitbang mode in the EEPROM first.

    Thanks to Steve Brown <sbrown@ewol.com> for the
    the information how to do it.

    The top nibble controls input/output and the bottom nibble
    controls the state of the lines set to output. The datasheet isn't clear
    what happens if you set a bit in the output register when that line is
    conditioned for input. This is described in more detail
    in the FT232R bitbang app note.

    BITMASK
    CBUS Bits
    3210 3210
    xxxx xxxx
    |    |------ Output Control 0->LO, 1->HI
    |----------- Input/Output   0->Input, 1->Output

    Example:
    All pins to output with 0 bit high: 0xF1 (11110001)
    Bits 0 and 1 to input, 2 and 3 to output and masked high: 0xCC (11001100)

    This program is distributed under the GPL, version 2
*/

#include <stdio.h>
#include <unistd.h>
#include <ftdi.h>

int main(int argc, char **argv)
{
    struct ftdi_context ftdic;
    int f,i;
    char buf[1];
    unsigned char bitmask;

    ftdi_init(&ftdic);

    f = ftdi_usb_open(&ftdic, 0x0403, 0x6001);
    if(f < 0 && f != -5) {
        fprintf(stderr, "unable to open ftdi device: %d (%s)\n", f, ftdi_get_error_string(&ftdic));
        exit(-1);
    }
    printf("ftdi open succeeded: %d\n",f);

    // enable CBUS
    bitmask = 0x71;
    printf("enabling CBUS mode\n");
    f = ftdi_set_bitmode(&ftdic, bitmask, 0x20); // cbus[0] as input (high), cbus[1-3] as output
    if (f < 0) {
        fprintf(stderr, "set_bitmode failed for 0x%x, error %d (%s)\n", bitmask, f, ftdi_get_error_string(&ftdic));
        exit(-1);
    }

    // write to CBUS
    buf[0] = 0xFF;
    f = ftdi_write_data(&ftdic, buf, 1);
    if(f < 0) {
        fprintf(stderr,"write failed for 0x%x, error %d (%s)\n", buf[0], f, ftdi_get_error_string(&ftdic));
        exit(-1);
    }

    // read CBUS
    f = ftdi_read_pins(&ftdic, &buf[0]);
    if (f < 0) {
        fprintf(stderr, "read_pins failed, error %d (%s)\n", f, ftdi_get_error_string(&ftdic));
        exit(-1);
    }

    printf("disabling bitbang mode\n");
    ftdi_disable_bitbang(&ftdic);

    ftdi_usb_close(&ftdic);
    ftdi_deinit(&ftdic);
}
