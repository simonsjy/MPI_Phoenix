INCLUDES=-I$(top_srcdir)/include -I$(top_srcdir)/include/worknode -I$(top_srcdir)/src/worknode/core

AM_LDFLAGS = -L$(top_srcdir)/lib  -L.
AM_CXXFLAGS = -Wall -O3 -DMMAP_POPULATE -fstrict-aliasing -Wstrict-aliasing  -D__x86__  -D_LINUX_
#AM_CFLAGS = -Wl,--rpath-libtool

sbin_PROGRAMS = test_system_debug  test_memory test_wordcount

test_system_debug_SOURCES = test_system_debug.cpp 
test_system_debug_LDADD = -lmympi -lmpich -lmpl -lpthread

test_memory_SOURCES = test_memory.cpp 
test_memory_LDADD = -lmympi -lmpich -lmpl  -lpthread

test_wordcount_SOURCES = test_wordcount.cpp  intraNode.cpp
test_wordcount_LDADD = -lmympi -lmpich -lmpl -lpthread 
