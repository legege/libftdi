/***************************************************************************
                             main.c  -  description
                           -------------------
    begin                : Mon Apr  7 12:05:22 CEST 2003
    copyright            : (C) 2003-2011 by Intra2net AG and the libftdi developers
    email                : opensource@intra2net.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 as     *
 *   published by the Free Software Foundation.                            *
 *                                                                         *
 ***************************************************************************/

/*
 TODO:
    - Use new eeprom get/set functions
    - Remove 128 bytes limit
    - Merge Uwe's eeprom tool. Current features:
        - Init eeprom defaults based upon eeprom type
        - Read -> Already there
        - Write -> Already there
        - Erase -> Already there
        - Decode on stdout
        - Ability to find device by PID/VID, product name or serial

 TODO nice-to-have:
    - Out-of-the-box compatibility with FTDI's eeprom tool configuration files
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <confuse.h>
#include <ftdi.h>
#include <ftdi_eeprom_version.h>

int str_to_cbus(char *str, int max_allowed)
{
    #define MAX_OPTION 14
    const char* options[MAX_OPTION] = {
     "TXDEN", "PWREN", "RXLED", "TXLED", "TXRXLED", "SLEEP",
     "CLK48", "CLK24", "CLK12", "CLK6",
     "IO_MODE", "BITBANG_WR", "BITBANG_RD", "SPECIAL"};
    int i;
    max_allowed += 1;
    if (max_allowed > MAX_OPTION) max_allowed = MAX_OPTION;
    for (i=0; i<max_allowed; i++) {
        if (!(strcmp(options[i], str))) {
            return i;
        }
    }
    printf("WARNING: Invalid cbus option '%s'\n", str);
    return 0;
}

int main(int argc, char *argv[])
{
    /*
    configuration options
    */
    cfg_opt_t opts[] =
    {
        CFG_INT("vendor_id", 0, 0),
        CFG_INT("product_id", 0, 0),
        CFG_BOOL("self_powered", cfg_true, 0),
        CFG_BOOL("remote_wakeup", cfg_true, 0),
        CFG_STR_LIST("chip_type", "{BM,R,other}", 0),
        CFG_BOOL("in_is_isochronous", cfg_false, 0),
        CFG_BOOL("out_is_isochronous", cfg_false, 0),
        CFG_BOOL("suspend_pull_downs", cfg_false, 0),
        CFG_BOOL("use_serial", cfg_false, 0),
        CFG_BOOL("change_usb_version", cfg_false, 0),
        CFG_INT("usb_version", 0, 0),
        CFG_INT("max_power", 0, 0),
        CFG_STR("manufacturer", "Acme Inc.", 0),
        CFG_STR("product", "USB Serial Converter", 0),
        CFG_STR("serial", "08-15", 0),
        CFG_STR("filename", "", 0),
        CFG_BOOL("flash_raw", cfg_false, 0),
        CFG_BOOL("high_current", cfg_false, 0),
        CFG_STR_LIST("cbus0", "{TXDEN,PWREN,RXLED,TXLED,TXRXLED,SLEEP,CLK48,CLK24,CLK12,CLK6,IO_MODE,BITBANG_WR,BITBANG_RD,SPECIAL}", 0),
        CFG_STR_LIST("cbus1", "{TXDEN,PWREN,RXLED,TXLED,TXRXLED,SLEEP,CLK48,CLK24,CLK12,CLK6,IO_MODE,BITBANG_WR,BITBANG_RD,SPECIAL}", 0),
        CFG_STR_LIST("cbus2", "{TXDEN,PWREN,RXLED,TXLED,TXRXLED,SLEEP,CLK48,CLK24,CLK12,CLK6,IO_MODE,BITBANG_WR,BITBANG_RD,SPECIAL}", 0),
        CFG_STR_LIST("cbus3", "{TXDEN,PWREN,RXLED,TXLED,TXRXLED,SLEEP,CLK48,CLK24,CLK12,CLK6,IO_MODE,BITBANG_WR,BITBANG_RD,SPECIAL}", 0),
        CFG_STR_LIST("cbus4", "{TXDEN,PWRON,RXLED,TXLED,TX_RX_LED,SLEEP,CLK48,CLK24,CLK12,CLK6}", 0),
        CFG_BOOL("invert_txd", cfg_false, 0),
        CFG_BOOL("invert_rxd", cfg_false, 0),
        CFG_BOOL("invert_rts", cfg_false, 0),
        CFG_BOOL("invert_cts", cfg_false, 0),
        CFG_BOOL("invert_dtr", cfg_false, 0),
        CFG_BOOL("invert_dsr", cfg_false, 0),
        CFG_BOOL("invert_dcd", cfg_false, 0),
        CFG_BOOL("invert_ri", cfg_false, 0),
        CFG_END()
    };
    cfg_t *cfg;

    /*
    normal variables
    */
    int _read = 0, _erase = 0, _flash = 0;
    unsigned char eeprom_buf[128];                  // TODO: Kill this and look for other hardcoded places of 128 bytes
    char *filename;
    int size_check;
    int i, argc_filename;
    FILE *fp;

    struct ftdi_context *ftdi = NULL;

    printf("\nFTDI eeprom generator v%s\n", EEPROM_VERSION_STRING);
    printf ("(c) Intra2net AG and the libftdi developers <opensource@intra2net.com>\n");

    if (argc != 2 && argc != 3)
    {
        printf("Syntax: %s [commands] config-file\n", argv[0]);
        printf("Valid commands:\n");
        printf("--read-eeprom  Read eeprom and write to -filename- from config-file\n");
        printf("--erase-eeprom  Erase eeprom\n");
        printf("--flash-eeprom  Flash eeprom\n");
        exit (-1);
    }

    if (argc == 3)
    {
        if (strcmp(argv[1], "--read-eeprom") == 0)
            _read = 1;
        if (strcmp(argv[1], "--erase-eeprom") == 0)
            _erase = 1;
        if (strcmp(argv[1], "--flash-eeprom") == 0)
            _flash = 1;

        argc_filename = 2;
    }
    else
    {
        argc_filename = 1;
    }

    if ((fp = fopen(argv[argc_filename], "r")) == NULL)
    {
        printf ("Can't open configuration file\n");
        exit (-1);
    }
    fclose (fp);

    cfg = cfg_init(opts, 0);
    cfg_parse(cfg, argv[argc_filename]);
    filename = cfg_getstr(cfg, "filename");

    if (cfg_getbool(cfg, "self_powered") && cfg_getint(cfg, "max_power") > 0)
        printf("Hint: Self powered devices should have a max_power setting of 0.\n");

    if ((ftdi = ftdi_new()) == 0)
    {
        fprintf(stderr, "Failed to allocate ftdi structure :%s \n",
                ftdi_get_error_string(ftdi));
        return EXIT_FAILURE;
    }

    ftdi_eeprom_initdefaults (ftdi, "Acme Inc.", "FTDI Chip", NULL);

    ftdi->eeprom->vendor_id = cfg_getint(cfg, "vendor_id");
    ftdi->eeprom->product_id = cfg_getint(cfg, "product_id");
    char *type = cfg_getstr(cfg, "chip_type");
    if (!strcmp(type, "BM")) {
        ftdi->type = TYPE_BM;
    } else if (!strcmp(type, "R")) {
        ftdi->type = TYPE_R;
    } else {
        ftdi->type = TYPE_AM;
    }

    ftdi->eeprom->self_powered = cfg_getbool(cfg, "self_powered");
    ftdi->eeprom->remote_wakeup = cfg_getbool(cfg, "remote_wakeup");
    ftdi->eeprom->max_power = cfg_getint(cfg, "max_power");

    ftdi->eeprom->in_is_isochronous = cfg_getbool(cfg, "in_is_isochronous");
    ftdi->eeprom->out_is_isochronous = cfg_getbool(cfg, "out_is_isochronous");
    ftdi->eeprom->suspend_pull_downs = cfg_getbool(cfg, "suspend_pull_downs");

    ftdi->eeprom->use_serial = cfg_getbool(cfg, "use_serial");
    ftdi->eeprom->use_usb_version = cfg_getbool(cfg, "change_usb_version");
    ftdi->eeprom->usb_version = cfg_getint(cfg, "usb_version");


    ftdi->eeprom->manufacturer = cfg_getstr(cfg, "manufacturer");
    ftdi->eeprom->product = cfg_getstr(cfg, "product");
    ftdi->eeprom->serial = cfg_getstr(cfg, "serial");
    ftdi->eeprom->high_current = cfg_getbool(cfg, "high_current");
    ftdi->eeprom->cbus_function[0] = str_to_cbus(cfg_getstr(cfg, "cbus0"), 13);
    ftdi->eeprom->cbus_function[1] = str_to_cbus(cfg_getstr(cfg, "cbus1"), 13);
    ftdi->eeprom->cbus_function[2] = str_to_cbus(cfg_getstr(cfg, "cbus2"), 13);
    ftdi->eeprom->cbus_function[3] = str_to_cbus(cfg_getstr(cfg, "cbus3"), 13);
    ftdi->eeprom->cbus_function[4] = str_to_cbus(cfg_getstr(cfg, "cbus4"), 9);
    int invert = 0;
    if (cfg_getbool(cfg, "invert_rxd")) invert |= INVERT_RXD;
    if (cfg_getbool(cfg, "invert_txd")) invert |= INVERT_TXD;
    if (cfg_getbool(cfg, "invert_rts")) invert |= INVERT_RTS;
    if (cfg_getbool(cfg, "invert_cts")) invert |= INVERT_CTS;
    if (cfg_getbool(cfg, "invert_dtr")) invert |= INVERT_DTR;
    if (cfg_getbool(cfg, "invert_dsr")) invert |= INVERT_DSR;
    if (cfg_getbool(cfg, "invert_dcd")) invert |= INVERT_DCD;
    if (cfg_getbool(cfg, "invert_ri")) invert |= INVERT_RI;
    ftdi->eeprom->invert = invert;

    if (_read > 0 || _erase > 0 || _flash > 0)
    {
        i = ftdi_usb_open(ftdi, ftdi->eeprom->vendor_id, ftdi->eeprom->product_id);

        if (i == 0)
        {
            printf("EEPROM size: %d\n", ftdi->eeprom->size);
        }
        else
        {
            printf("Unable to find FTDI devices under given vendor/product id: 0x%X/0x%X\n", ftdi->eeprom->vendor_id, ftdi->eeprom->product_id);
            printf("Error code: %d (%s)\n", i, ftdi_get_error_string(ftdi));
            printf("Retrying with default FTDI id.\n");

            i = ftdi_usb_open(ftdi, 0x0403, 0x6001);
            if (i != 0)
            {
                printf("Error: %s\n", ftdi->error_str);
                exit (-1);
            }
        }
    }

    if (_read > 0)
    {
        printf("FTDI read eeprom: %d\n", ftdi_read_eeprom(ftdi));

        ftdi_eeprom_decode(ftdi, 0);
        /* Debug output */
        /*
        const char* chip_types[] = {"other", "BM", "R"};
        printf("vendor_id = \"%04x\"\n", eeprom->vendor_id);
        printf("product_id = \"%04x\"\n", eeprom->product_id);
        printf("chip_type = \"%s\"\n",
          (eeprom->chip_type > 0x06) || (eeprom->chip_type & 0x01) ? "unknown":
          chip_types[eeprom->chip_type>>1]);
        printf("self_powered = \"%s\"\n", eeprom->self_powered?"true":"false");
        printf("remote_wakeup = \"%s\"\n", eeprom->remote_wakeup?"true":"false");
        printf("max_power = \"%d\"\n", eeprom->max_power);
        printf("in_is_isochronous = \"%s\"\n", eeprom->in_is_isochronous?"true":"false");
        printf("out_is_isochronous = \"%s\"\n", eeprom->out_is_isochronous?"true":"false");
        printf("suspend_pull_downs = \"%s\"\n", eeprom->suspend_pull_downs?"true":"false");
        printf("use_serial = \"%s\"\n", eeprom->use_serial?"true":"false");
        printf("change_usb_version = \"%s\"\n", eeprom->change_usb_version?"true":"false");
        printf("usb_version = \"%d\"\n", eeprom->usb_version);
        printf("manufacturer = \"%s\"\n", eeprom->manufacturer);
        printf("product = \"%s\"\n", eeprom->product);
        printf("serial = \"%s\"\n", eeprom->serial);
        */

        if (filename != NULL && strlen(filename) > 0)
        {
            FILE *fp = fopen (filename, "wb");
            fwrite (eeprom_buf, 1, 128, fp);
            fclose (fp);
        }
        else
        {
            printf("Warning: Not writing eeprom, you must supply a valid filename\n");
        }

        goto cleanup;
    }

    if (_erase > 0)
    {
        printf("FTDI erase eeprom: %d\n", ftdi_erase_eeprom(ftdi));
    }

    size_check = ftdi_eeprom_build(ftdi);

    if (size_check == -1)
    {
        printf ("Sorry, the eeprom can only contain 128 bytes (100 bytes for your strings).\n");
        printf ("You need to short your string by: %d bytes\n", size_check);
        goto cleanup;
    } else if (size_check < 0) {
        printf ("ftdi_eeprom_build(): error: %d\n", size_check);
    }
    else
    {
        printf ("Used eeprom space: %d bytes\n", 128-size_check);
    }

    if (_flash > 0)
    {
        if (cfg_getbool(cfg, "flash_raw"))
        {
            if (filename != NULL && strlen(filename) > 0)
            {
                FILE *fp = fopen(filename, "rb");
                fread(eeprom_buf, 1, 128, fp);
                fclose(fp);
            }
        }
        printf ("FTDI write eeprom: %d\n", ftdi_write_eeprom(ftdi));
    }

    // Write to file?
    if (filename != NULL && strlen(filename) > 0)
    {
        fp = fopen(filename, "w");
        if (fp == NULL)
        {
            printf ("Can't write eeprom file.\n");
            exit (-1);
        }
        else
            printf ("Writing to file: %s\n", filename);

        fwrite(eeprom_buf, 128, 1, fp);
        fclose(fp);
    }

cleanup:
    if (_read > 0 || _erase > 0 || _flash > 0)
    {
        printf("FTDI close: %d\n", ftdi_usb_close(ftdi));
    }

    ftdi_deinit (ftdi);

    cfg_free(cfg);

    printf("\n");
    return 0;
}
