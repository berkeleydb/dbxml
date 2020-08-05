#! /bin/sh

usage() {
	cat <<EOF
Usage: $0 [--xqts <path>] [--xquts <path>] --keepgoing
  --xqts <path>:    Where to find XQTSCatalog.xml
  --xquts <path>:   Where to find XQUTSCatalog.xml
  --keepgoing:      Set this to ignore failures and keep running tests
  All paths must be absolute.  If run without --xqts or --xquts only the 
  local (BDB XML specific) query tests will be run.  Current XQTS version is
  1.0.2 and is at:
     http://www.w3.org/XML/Query/test-suite/
  Current XQUTS can be found here:
     http://dev.w3.org/2007/xquery-update-10-test-suite/
EOF
}

KEEPGOING="false"

# Parse arguments
for option
do
	# If the previous option needs an argument, assign it.
	if test -n "$prev" ; then
		eval "$prev=\$option"
		prev=
		continue
	fi

	case "$option" in
	-*=*) optarg=`echo "$option" | sed 's/[-_a-zA-Z0-9]*=//'` ;;
	*) optarg= ;;
	esac

	case "$option" in
	-xqts|--xqts)       prev=XQTS_LOC ;;
	-xquts|--xquts)     prev=XQTUS_LOCATION ;;
	-keep*|--keep*)     KEEPGOING="true" ;;
	-h|--help|-?)		usage ; exit 0 ;;
	*)	echo "$0: error: $option invalid option." 1>&2
		echo "Run with --help to see usage." 1>&2
		exit 1
		;;
	esac
done

SCRIPT_PATH=`pwd`/`dirname $0`

if [ ! -e xqts_env ]; then
    mkdir xqts_env
fi

if [ ! -e xqts_env_t ]; then
    mkdir xqts_env_t
fi

#if [ "$XQTS_LOC" = "" -a "$XQUTS_LOC" = "" ]; then
#    usage
#    exit 1
#fi

if [ "$XQTS_LOC" != "" -a ! -f "$XQTS_LOC" ]; then
    echo "The XQuery test suite cannot be found at location: $XQTS_LOC"
    usage
    exit 1
fi

if [ "$XQUTS_LOC" != "" -a ! -f "$XQUTS_LOC" ]; then
    echo "The XQuery Update test suite cannot be found at location: $XQUTS_LOC"
    usage
    exit 1
fi

if [ -f "$XQTS_LOC" ]; then
    echo "XQuery Conformance Test Suite (NLS+)"
    echo "========================================================================"
    echo "location: $XQTS_LOC"
    echo "errors: xqts-latest-errors.NLS+"
    ./dbxml_xqts_runner -e $SCRIPT_PATH/xqts-errors.xml -E xqts-latest-errors.NLS+.xml -h xqts_env -c xqts -s NLS+ $XQTS_LOC 2>xqts-latest-errors.NLS+
    if [ $? != 0 ]; then echo "TESTS FAILED"; if [ $KEEPGOING != "true" ]; then exit; fi; fi
    
    echo "XQuery Conformance Test Suite (NLS)"
    echo "========================================================================"
    echo "location: $XQTS_LOC"
    echo "errors: xqts-latest-errors.NLS"
    ./dbxml_xqts_runner -e $SCRIPT_PATH/xqts-errors.xml -E xqts-latest-errors.NLS.xml -h xqts_env -c xqts -s NLS $XQTS_LOC 2>xqts-latest-errors.NLS
    if [ $? != 0 ]; then echo "TESTS FAILED"; if [ $KEEPGOING != "true" ]; then exit; fi; fi
    
    echo "XQuery Conformance Test Suite (DLS+)"
    echo "========================================================================"
    echo "location: $XQTS_LOC"
    echo "errors: xqts-latest-errors.DLS+"
    ./dbxml_xqts_runner -e $SCRIPT_PATH/xqts-errors.xml -E xqts-latest-errors.DLS+.xml -h xqts_env -c xqts -s DLS+ $XQTS_LOC 2>xqts-latest-errors.DLS+
    if [ $? != 0 ]; then echo "TESTS FAILED"; if [ $KEEPGOING != "true" ]; then exit; fi; fi
    
    echo "XQuery Conformance Test Suite (DLS)"
    echo "========================================================================"
    echo "location: $XQTS_LOC"
    echo "errors: xqts-latest-errors.DLS"
    ./dbxml_xqts_runner -e $SCRIPT_PATH/xqts-errors.xml -E xqts-latest-errors.DLS.xml -h xqts_env -c xqts -s DLS $XQTS_LOC 2>xqts-latest-errors.DLS
    if [ $? != 0 ]; then echo "TESTS FAILED"; if [ $KEEPGOING != "true" ]; then exit; fi; fi

fi
    
if [ -f "$XQUTS_LOC" ]; then
    echo "XQuery Update Conformance Test Suite (Non-transacted)"
    echo "========================================================================"
    echo "location: $XQUTS_LOC"
    echo "errors: xquts-latest-errors"
    ./dbxml_xqts_runner -E xquts-latest-errors.xml -h xqts_env -c xquts -p $XQUTS_LOC 2>xquts-latest-errors
    if [ $? != 0 ]; then echo "TESTS FAILED"; if [ $KEEPGOING != "true" ]; then exit; fi; fi
    
    echo "XQuery Update Conformance Test Suite (Transacted)"
    echo "========================================================================"
    echo "location: $XQUTS_LOC"
    echo "errors: xquts-latest-errors.txn"
    ./dbxml_xqts_runner -E xquts-latest-errors.txn.xml -h xqts_env_t -c xquts -p -t $XQUTS_LOC 2>xquts-latest-errors.txn
    if [ $? != 0 ]; then echo "TESTS FAILED"; if [ $KEEPGOING != "true" ]; then exit; fi; fi
fi

echo "DB XML Query Test Suite"
echo "========================================================================"
echo "location: $SCRIPT_PATH/queries/queries.xml"
echo "errors: query-latest-errors"
./dbxml_xqts_runner -E query-latest-errors.xml -h xqts_env -c query $SCRIPT_PATH/queries/queries.xml 2>query-latest-errors
if [ $? != 0 ]; then echo "TESTS FAILED"; if [ $KEEPGOING != "true" ]; then exit; fi; fi


echo "DB XML Update Test Suite (Non-transacted)"
echo "========================================================================"
echo "location: $SCRIPT_PATH/updates/updates.xml"
echo "errors: update-latest-errors"
./dbxml_xqts_runner -E update-latest-errors.xml -h xqts_env -c update $SCRIPT_PATH/updates/updates.xml 2>update-latest-errors
if [ $? != 0 ]; then echo "TESTS FAILED"; if [ $KEEPGOING != "true" ]; then exit; fi; fi

echo "DB XML Update Test Suite (Transacted)"
echo "========================================================================"
echo "location: $SCRIPT_PATH/updates/updates.xml"
echo "errors: update-latest-errors.txn"
./dbxml_xqts_runner -E update-latest-errors.txn.xml -h xqts_env_t -c update -t $SCRIPT_PATH/updates/updates.xml 2>update-latest-errors.txn
if [ $? != 0 ]; then echo "TESTS FAILED"; if [ $KEEPGOING != "true" ]; then exit; fi; fi

