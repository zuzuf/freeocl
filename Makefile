
TOPDIR=$(shell pwd)
BUILDDIR=${TOPDIR}/build
J=2

all: configure
	cd ${BUILDDIR} && make -j$J
	echo "All done"
.PHONY : all

pre-build:
	mkdir -p ${BUILDDIR}
.PHONY : pre-build

configure: pre-build
	cd ${BUILDDIR} && cmake -i ${TOPDIR}
.PHONY : configure

debug: pre-build
	cd ${BUILDDIR} && cmake -DCMAKE_BUILD_TYPE=Debug ${TOPDIR} && make -j$J
.PHONY : debug

release: pre-build
	cd ${BUILDDIR} && cmake -DCMAKE_BUILD_TYPE=Release ${TOPDIR} && make -j$J
.PHONY : release

package: distclean pre-build
	cd ${BUILDDIR} && cmake -DCMAKE_BUILD_TYPE=Release ${TOPDIR} && make -j$J package
.PHONY : package

clean:
	cd ${BUILDDIR} && make clean
.PHONY : clean

distclean: clean
	rm -rf ${BUILDDIR}
.PHONY : distclean
