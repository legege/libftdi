CLEANFILES = rm -rf html man

if HAVE_DOXYGEN
MANUALS = html/index.html
else
MANUALS =
endif

all: $(MANUALS)

html/index.html: $(top_srcdir)/src/*.c $(top_srcdir)/src/*.h $(top_srcdir)/ftdipp/*.cpp $(top_srcdir)/ftdipp/*.hpp $(top_srcdir)/examples/*.c
	$(DOXYGEN)
