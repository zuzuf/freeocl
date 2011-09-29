#!/bin/sh

#set -x
#set -e

TOP_DIR=`pwd`
SCRIPT_DIR=${TOP_DIR}/`dirname $0`
CACHE_DIR=${HOME}/.cache/freeocl
: ${BUILD_DIR:="${TOP_DIR}/build"}
SDK_VER=2.5
SDK_BIT=`getconf LONG_BIT`
TIMEOUT="30"

case "$( uname -m )" in
	i?86) export ARCH=x86 ;;
	arm*) export ARCH=arm ;;
	# Unless $ARCH is already set, use uname -m for all other archs:
	*) export ARCH=$( uname -m ) ;;
esac

SDK_DIR=AMD-APP-SDK-v${SDK_VER}-RC2-lnx${SDK_BIT}
SDK_TAR=${SDK_DIR}.tgz
SDK_ARCHIVE=AMD-APP-SDK-v${SDK_VER}-lnx${SDK_BIT}.tgz
SDK_URL=http://developer.amd.com/Downloads/${SDK_ARCHIVE}

TESTS_DIR=${BUILD_DIR}/${SDK_DIR}/samples/opencl/bin/${ARCH}
RESULTS_DIR=${BUILD_DIR}/${SDK_DIR}/_RESULTS
MAKE_DONE=${BUILD_DIR}/make_done
PATCH_DONE=${BUILD_DIR}/patch_done

runWithTimeout()
{
    if [ $# -lt 2 ]; then
        echo "usage: $0 <timeout ms> <commandline>"
        return 1
    fi
    timeout=$1
    shift

    # Start the command.
    "$@" &
    cmdPid=$!

    # Start the timeout process.
    ( sleep $timeout; echo "$1 timed out (pid $cmdPid)"; kill $cmdPid ) &
    sleepPid=$!

    # Don't leak the children if we're interrupted by ^C etc.
    trap "kill $cmdPid $sleepPid; exit 5" INT TERM EXIT

    # Block until the command exits or times out.
    wait $cmdPid
    cmdStatus=$?

    # Clear out the ^C trap.
    trap - INT TERM EXIT

    if [ $cmdStatus -le 128 ] ; then
        # The command exited on its own; kill the timeout process.
        # This will kill the background shell, not the sleep
        # process started by it, so the sleep itself may hang around
        # for a while. But, since the subshell will be gone when it
        # exits, nothing will try to kill $cmdPid.
        kill $sleepPid
    fi

    return $cmdStatus
}

if [ ! -d "${BUILD_DIR}" ]; then
	echo "You do not have BUILD folder. Run make"
	exit 1
fi

mkdir -p ${CACHE_DIR}

if [ ! -d "${BUILD_DIR}/${SDK_DIR}" ]; then

cd ${CACHE_DIR}
if [ ! -f ${SDK_ARCHIVE} ]; then
	wget ${SDK_URL}
fi
	tar -xvf ${SDK_ARCHIVE}
	cd ${BUILD_DIR}
	tar -xvf ${CACHE_DIR}/${SDK_TAR}
fi

mkdir -p ${RESULTS_DIR}
if [ ! -f ${MAKE_DONE} ]; then
	cd ${BUILD_DIR}/${SDK_DIR}
	
	if [ ! -f ${PATCH_DONE} ]; then
		patch -p0 < ${SCRIPT_DIR}/openclsdkdefs_mk.patch 
		patch -p0 < ${SCRIPT_DIR}/openclsdkrules_mk.patch
		touch ${PATCH_DONE}
	fi
	
	logfile=${RESULTS_DIR}/make.log
	export FOCL_BUILD_DIR=${BUILD_DIR}
	echo "make ..."
	make >${logfile} 2>&1
	if [ $? -eq 0 ]; then
		touch ${MAKE_DONE}
		echo "done."
	fi
fi

: ${TESTS:=`ls  ${TESTS_DIR} | grep -v "\." | xargs`}

export LD_LIBRARY_PATH=${BUILD_DIR}/src:${BUILD_DIR}/src/icd:${LD_LIBRARY_PATH}

cd "${TOPDIR}"

echo "Will run tests:"
echo "${TESTS}"
echo "You can modify the list by running TESTS=\"test1 test2\" $0"

total=0
failed=0
timed=0

for f in ${TESTS}
do
	requires_gl=`ldd ${TESTS_DIR}/$f  | grep libGL`
	if [ -z "${requires_gl}" ]; then
		logfile=${RESULTS_DIR}/$f.log
		
		echo "$f ..."
		
		runWithTimeout ${TIMEOUT} \
		${TESTS_DIR}/$f --device cpu >${logfile} 2>&1;
		
		case "$?" in
			0) 
				echo "succeeded" 
			;;
			1) 
				echo "failed" 
				failed=`expr $failed + 1`
			;;
			*) 
				echo "timed out (${TIMEOUT} seconds)" 
				timed=`expr $timed + 1`
			;;
		esac
		total=`expr $total + 1`
	else
		echo "$f requires GL"
	fi
done;

echo "$0 finished."
echo "total/failed/timed out"
echo "${total}/${failed}/${timed}"
echo "Look for the results in ${RESULTS_DIR}"
