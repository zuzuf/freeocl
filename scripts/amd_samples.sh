#!/bin/sh

#set -x
#set -e

SCRIPT_DIR=`dirname $0`
TOP_DIR=`pwd`
CACHE_DIR=${TOP_DIR}/.cache
: ${BUILD_DIR:="${TOP_DIR}/build"}
SDK_VER=2.5
SDK_BIT=`getconf LONG_BIT`
TIMEOUT="10"

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
	return 1
    fi

    return 0
}

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

: ${TESTS:=`ls  ${TESTS_DIR} | grep -v "\." | xargs`}

if [ ! -d ${TESTS_DIR} ]; then
	cd ${BUILD_DIR}/${SDK_DIR}
	#do patching here

	make
fi

mkdir -p ${RESULTS_DIR}
export LD_LIBRARY_PATH=${BUILD_DIR}/src:${BUILD_DIR}/src/icd:${LD_LIBRARY_PATH}

for f in ${TESTS}
do
	requires_gl=`ldd ${TESTS_DIR}/$f  | grep libGL`
	if [ -z "${requires_gl}" ]; then
		logfile=${RESULTS_DIR}/$f.log
		
		runWithTimeout ${TIMEOUT} \
		${TESTS_DIR}/$f --device cpu >${logfile} 2>&1 \
		&& echo "$f succeeded" || echo "$f failed";
		if [ $? -eq 1 ] ; then
			echo "$f timed out (${TIMEOUT} seconds)"
		fi
	else
		echo "$f requires GL"
	fi
done;

echo "$0 finished successfully. Look for the results in ${RESULTS_DIR}"
