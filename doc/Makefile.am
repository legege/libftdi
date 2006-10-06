EXTRA_DIST =

# I grabbed this same hack from the VACM docs/Makfile.am
CLEANFILES = rm -rf html man

if HAVE_DOXYGEN
MANUALS = html/index.html
else
MANUALS =
endif

all: $(MANUALS)

html/index.html: ../src/*.c ../src/*.h
	$(DOXYGEN)
