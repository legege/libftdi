# the library search path.
lib_LTLIBRARIES =  libftdi.la
libftdi_la_SOURCES =  ftdi.c
include_HEADERS =  ftdi.h

# Note:  If you specify a:b:c as the version in the next line,
#  the library that is made has version (a-c).c.b.  In this
#  example, the version is 2.1.2. (3:2:1)
libftdi_la_LDFLAGS = -version-info 18:0:17
libftdi_la_LIBADD = @USB_LIBS@

EXTRA_DIST = CMakeLists.txt
