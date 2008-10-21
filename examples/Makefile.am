METASOURCES = AUTO
INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/ftdipp
LDADD = $(top_builddir)/src/libftdi.la


if HAVE_LIBFTDIPP
examples_libftdipp = find_all_pp
else
examples_libftdipp =
endif


bin_PROGRAMS = simple \
	bitbang       \
	bitbang2      \
	bitbang_ft2232 \
	bitbang_cbus \
	find_all \
	$(examples_libftdipp)

# Don't install the example files
AM_LDFLAGS = -no-install

simple_SOURCES = simple.c
bitbang_SOURCES = bitbang.c
bitbang2_SOURCES = bitbang2.c
bitbang_ft2232_SOURCES = bitbang_ft2232.c
bitbang_cbus_SOURCES = bitbang_cbus.c
find_all_SOURCES = find_all.c

if HAVE_LIBFTDIPP
find_all_pp_SOURCES = find_all_pp.cpp
find_all_pp_LDADD = $(top_builddir)/ftdipp/libftdipp.la $(LDADD)
endif
