#EXTRA_DIST = reconf Makefile.autogen Makefile.autogensimp libtoolcreate.sh
EXTRA_DIST =
SUBDIRS = www mod

versiondir = $(prefix)
version_DATA = version  abc.ko boa.conf


BUILT_SOURCES = version
CLEANFILES = $(BUILT_SOURCES)

version:Makefile
	echo "Embedded MRU system info" > $@;
	echo "=====================================" >> $@;
	echo "Product Name:   MRU"  >> $@;
	echo "Hardware_Version :   0x0100"  >> $@;
	echo "Software_Version :   0x0100"  >> $@;
	echo "Fpga_Version :   0x0100"  >> $@;
	echo "=====================================" >> $@;
	echo "generate at:    "`date -R` >> $@;
	echo "compiled by:    "`whoami`  >> $@;
	echo "host:           "`hostname` >> $@;
	echo "compiled with:" >> $@;
	echo "=====================================" >> $@;
	grep '$$.*configure' ../config.log >> $@;
	echo "=====================================" >> $@;
	echo "gcc info:" >> $@;
	echo "=====================================" >> $@;
	$(CC) -v 2>> $@;
	echo "=====================================" >> $@;

#install-data-hook:
#	if ! test -e $(prefix)/images; then \
#	mkdir $(prefix)/images; \
#	fi

