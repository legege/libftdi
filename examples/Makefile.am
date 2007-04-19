METASOURCES = AUTO
INCLUDES = -I$(top_srcdir)/src
LDADD = $(top_builddir)/src/libftdi.la

bin_PROGRAMS = simple \
	bitbang       \
	bitbang2      \
	bitbang_ft2232 \
	find_all

# Don't install the example files
AM_LDFLAGS = -no-install

simple_SOURCES = simple.c
bitbang_SOURCES = bitbang.c
bitbang2_SOURCES = bitbang2.c
bitbang_ft2232_SOURCES = bitbang_ft2232.c
find_all_SOURCES = find_all.c
