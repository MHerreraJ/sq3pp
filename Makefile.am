ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src

if BUILD_EXAMPLES
SUBDIRS += example
endif

# Include headers in distribution
sq3ppincludedir = $(includedir)/sq3pp
sq3ppinclude_HEADERS = \
	include/sq3pp/Database.h \
	include/sq3pp/Exception.h \
	include/sq3pp/Statement.h \
	include/sq3pp/Transaction.h

# Extra files to distribute
EXTRA_DIST = README.md LICENSE

# pkg-config file
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = sq3pp.pc

# Clean generated files
CLEANFILES = $(pkgconfig_DATA)

# Generate pkg-config file
sq3pp.pc: Makefile
	@echo 'prefix=$(prefix)' > $@
	@echo 'exec_prefix=$(exec_prefix)' >> $@
	@echo 'libdir=$(libdir)' >> $@
	@echo 'includedir=$(includedir)' >> $@
	@echo '' >> $@
	@echo 'Name: sq3pp' >> $@
	@echo 'Description: C++ wrapper for SQLite3' >> $@
	@echo 'Version: $(VERSION)' >> $@
	@echo 'Requires:' >> $@
	@echo 'Libs: -L$${libdir} -lsq3pp' >> $@
	@echo 'Libs.private: -lsqlite3' >> $@
	@echo 'Cflags: -I$${includedir}' >> $@
