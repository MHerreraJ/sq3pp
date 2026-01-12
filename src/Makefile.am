# Library to build
lib_LTLIBRARIES = libsq3pp.la

# Library sources
libsq3pp_la_SOURCES = \
	Database.cpp \
	Statement.cpp \
	Transaction.cpp

# Include paths
libsq3pp_la_CPPFLAGS = -I$(top_srcdir)/include

# Compiler flags
libsq3pp_la_CXXFLAGS = -std=c++17 -Wall -Wextra

# Library dependencies
libsq3pp_la_LIBADD = -lsqlite3

# Version info (current:revision:age)
# See https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
libsq3pp_la_LDFLAGS = -version-info 1:0:0
