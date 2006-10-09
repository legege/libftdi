CLEANFILES = rm -rf html man

if HAVE_DOXYGEN
MANUALS = html/index.html
else
MANUALS =
endif

all: $(MANUALS)

html/index.html: ../src/*.c ../src/*.h ../examples/*.c
	$(DOXYGEN)
