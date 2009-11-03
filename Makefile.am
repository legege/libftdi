# not a GNU package. You can remove this line, if
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign 1.4

SUBDIRS = src bindings $(LIBFTDI_MODULES) examples doc

EXTRA_DIST = libftdi.spec libftdi.spec.in COPYING.LIB COPYING.GPL LICENSE \
             README AUTHORS ChangeLog libftdi-config.in CMakeLists.txt \
             packages COPYING-CMAKE-SCRIPTS FindUSB.cmake README.mingw \
             Toolchain-mingw32.cmake Toolchain-Crossbuild32.cmake

bin_SCRIPTS = libftdi-config

# Install the pkg-config file:
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libftdi.pc $(LIBFTDI_MODULES_PKGCONFIG)
