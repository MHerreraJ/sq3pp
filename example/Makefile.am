noinst_PROGRAMS = example

example_SOURCES = main.cpp
example_CXXFLAGS = -I$(top_srcdir)/include $(LIBSQLITE3_CFLAGS)
example_LDADD = $(top_builddir)/src/.libs/libsq3pp.a $(LIBSQLITE3_LIBS)
example_LDFLAGS = -all-static
