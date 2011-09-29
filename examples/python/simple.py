"""Python example program.

Small program to demonstrate the usage
of the swig generated python wrapper

You need to build and install the wrapper first"""

import ftdi

def main():
    """Main program"""
    context = ftdi.ftdi_context()
    ftdi.ftdi_init(context)

    version_info = ftdi.ftdi_get_library_version()
    print("[FTDI version] major: %d, minor: %d, micro: %d" \
               ", version_str: %s, snapshot_str: %s" %
               (version_info.major, version_info.minor, version_info.micro,
               version_info.version_str, version_info.snapshot_str))

    print("ftdi_open(): %d" % ftdi.ftdi_usb_open(context, 0x403, 0x6010))
    print("ftdi_set_baudrate(): %d" % ftdi.ftdi_set_baudrate(context, 9600))

    ftdi.ftdi_deinit(context)

main()
