####### kdevelop will overwrite this part!!! (begin)##########
lib_LTLIBRARIES = libftdi.la


## INCLUDES were found outside kdevelop specific part


LDFLAGS =  

libftdi_la_METASOURCES = AUTO

libftdi_la_SOURCES = ftdi.c 


EXTRA_DIST = ftdi.c ftdi.h 

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(prefix)/include/
	$(INSTALL_DATA) $(srcdir)/ftdi.h $(DESTDIR)$(prefix)/include/ftdi.h

uninstall-local:
	-rm -f $(DESTDIR)$(prefix)/include/ftdi.h

####### kdevelop will overwrite this part!!! (end)############
# set the include path found by configure
INCLUDES= $(all_includes)

# Note:  If you specify a:b:c as the version in the next line,
#  the library that is made has version (a-c).c.b.  In this
#  example, the version is 2.1.2. (3:2:1)
libftdi_la_LDFLAGS = -version-info 3:0:3 $(all_libraries) 
                                                                                
