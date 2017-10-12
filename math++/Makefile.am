#include $(top_srcdir)/niceprint.mak

AM_CXXFLAGS = -pedantic -ansi -Wall -Wno-long-long

lib_LTLIBRARIES = libmath++.la

libmath___la_SOURCES = utils.cpp
libmath___la_LDFLAGS = -version-info @MATH_VERSION_INFO@

mathinc_HEADERS = \
	nodes.h nodes.tcc \
	reader.h reader.tcc \
	printer.h printer.tcc \
	calculator.h calculator.tcc \
	derive.h derive.tcc \
	simplifier.h simplifier.tcc \
	expander.h expander.tcc \
	library.h library.tcc \
	matcher.h matcher.tcc \
	utils.h utils.tcc \
	visitor.h error.h 

mathincdir = $(includedir)/math++

install-exec-hook:
	$(mkinstalldirs) $(DESTDIR)$(mathincdir)

# this is just a test program wich is needed for testing newly implemented things
noinst_PROGRAMS = t1
t1_SOURCES = t1.cpp
t1_LDADD   = libmath++.la
