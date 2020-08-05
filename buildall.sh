#!/bin/sh
#
# This script builds a self-contained installed tree of
# Berkeley DB XML and its dependent libraries.
#
# By default, this script builds this configuration:
#  o no debug or diagnostics
#  o no Java
#  o no Tcl or testing config
#
# The default installation directory is:
#  o ./install
#
# Assumptions (configurable):

# CDPATH gets in the way
unset CDPATH

DBXML_DIR_DEFAULT=dbxml
XERCES_DIR_DEFAULT=xerces-c-src
XQILLA_DIR_DEFAULT=xqilla
DB_DIR_DEFAULT=db-4.8.26

XERCES_DEPDOM="libxerces-depdom.*"
XERCES_TO_REMOVE="$XERCES_DEPDOM libxerces-c.*"

DBXML_INSTALL_DEFAULT=./install
DB_INSTALL_DEFAULT=./install
XERCES_INSTALL_DEFAULT=./install
XQILLA_INSTALL_DEFAULT=./install

#
# There is no dependency checking in this script, so if it is called more than
# once, the configure steps are performed unconditionally.
#
usage() {
	cat <<EOF
Usage: $0 [options]
  --help                                 print this help message
  -c <C compiler name>                   [gcc]
  -x <C++ compiler name>                 [g++]
  -m <make command>                      [make]
  -b <bits to build: 32 or 64>           [32]
  --no-configure:                        do not re-run configure steps.  Assumes
                                           configuration has been run previously
  --clean:                               perform 'make clean' on all libraries
  --distclean:                           perform 'make distclean' on all
                                           libraries
  --prefix=<path>:                       set common installation prefix
                                           [./install]
  --enable-debug:                        debug build
  --enable-java:                         Java support
  --with-tcl=<path>:                     Tcl support
  --enable-test:                         Test suite support
                                           forces --enable-debug and requires
                                           either --with-tcl --enable-java
  --with-junit:                          Path to a JUnit (4) .jar file.
                                         Required if both --enable-test and 
                                         --enable-java are specified.
  --enable-dependencies:                 Build include file dependencies for
                                         dbxml.  May only work for g++.
  --enable-perl:                         Perl support.  Requires all
                                           libraries built and installed
  --perl-installdir=<path>               Path to Perl installation directory, used
                                           as PREFIX in the command
                                           perl Makefile.PL
                                           Default is the Perl site installation
                                           which may require super-user privilege.
                                           If used you may have to configure your
                                           environment to allow Perl to find the
                                           installed modules.
  --disable-rpath:                       Patch libtool to avoid the use of rpath
                                         in the libraries produced.
  --with-dbxml=<path>:                   use Berkeley DB XML sources in <path>
  --with-dbxml-conf=<flags>:             pass flags to DB XML configure
  --with-dbxml-prefix=<path>:            installation prefix for DB XML
                                           [$DBXML_INSTALL_DEFAULT]
  --with-berkeleydb=<path>:              use Berkeley DB sources in <path>
  --with-berkeleydb-conf=<flags>:        pass flags to DB configure
  --with-berkeleydb-prefix=<path>:       installation prefix for DB.  This
                                         option can also be used with 
                                         --build-one=dbxml to point to an
                                         existing Berkeley DB installation
                                           [$DB_INSTALL_DEFAULT]
  --with-xerces=<path>:                  use Xerces-C++ sources in <path>
  --with-xerces-conf=<configure flags>:  pass flags to Xerces-C++ configure
  --with-xerces-prefix=<path>:           installation prefix for Xerces
                                           [$XERCES_INSTALL_DEFAULT]
  --with-xqilla=<path>:                  use XQilla sources in <path>
  --with-xqilla-conf=<configure flags>:  pass flags to XQilla configure
  --with-xqilla-prefix=<path>:           installation prefix for XQilla
                                           [$XQILLA_INSTALL_DEFAULT]
  --with-configure-env=<env>:            Add environment to be set for calls to
                                         configure for DB, XQilla and BDB XML.  E.g.
                                         --with-configure-env="CXXFLAGS='someflag'"
                                         May be specified more than once.
  --build-one=<library>                  build only the specified library,
                                         which is one of berkeleydb, xerces,
                                         xqilla, dbxml, or perl
EOF
}

HOST_OS=`uname -s`

# Don't override user's settings for these common environment variables.
CC=${CC-gcc}
CXX=${CXX-g++}
MAKE=${MAKE-make}
BITS=32

DBXML_DIR=$DBXML_DIR_DEFAULT
XERCES_DIR=$XERCES_DIR_DEFAULT
XQILLA_DIR=$XQILLA_DIR_DEFAULT
DB_DIR=$DB_DIR_DEFAULT

DBXML_INSTALL_DIR=$DBXML_INSTALL_DEFAULT
DB_INSTALL_DIR=$DB_INSTALL_DEFAULT
XERCES_INSTALL_DIR=$XERCES_INSTALL_DEFAULT
XQILLA_INSTALL_DIR=$XQILLA_INSTALL_DEFAULT
XQILLA_BUILD_DIR=$XQILLA_DIR/build
DBXML_BUILD_DIR=$DBXML_DIR/build_unix

CONF_ENV=

DBXML_CONF_ARGS=
DB_CONF_ARGS=
XQILLA_CONF_ARGS=
XERCES_CONF_ARGS=

DBXML_CONF_XARGS=
DB_CONF_XARGS=
XQILLA_CONF_XARGS=
XERCES_CONF_XARGS=--disable-pretty-make
DO_CONFIGURE="yes"
# Note: a --no-install flag might make sense, except that
# it would cause builds to fail.  E.g. XQilla, and DB XML
# build against the *installed* location of Xerces, not source.
DO_INSTALL="yes"
DO_CLEAN="no"
DEBUG_ENABLED="no"
TCL_ENABLED="no"
JAVA_ENABLED="no"
TEST_ENABLED="no"
BUILD_ONE="no"
BUILD_PERL="no"
PERL_INSTALLDIR=""
PERL="perl"

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
	-c)	prev=CC ;;
	-x)	prev=CXX ;;
	-m)	prev=MAKE ;;
	-b)	prev=BITS ;;
	--no-configure)
		DO_CONFIGURE="no"
		;;
	--clean)
		DO_CLEAN="clean"
		;;
	--distclean)
		DO_CLEAN="distclean"
		;;
	--prefix=*)
		DBXML_INSTALL_DIR="$optarg"
		DB_INSTALL_DIR="$optarg"
		XQILLA_INSTALL_DIR="$optarg"
		XERCES_INSTALL_DIR="$optarg"
		;;
	--enable-test)
		# don't add enable-test to DB unless also enabling tcl
		# handle that below...
		#DB_CONF_ARGS="$DB_CONF_ARGS $option"
		DBXML_CONF_ARGS="$DBXML_CONF_ARGS $option"
		TEST_ENABLED="yes"
		;;
	--enable-debug)
		DB_CONF_ARGS="$DB_CONF_ARGS --enable-debug"
		DBXML_CONF_ARGS="$DBXML_CONF_ARGS --enable-debug"
		XERCES_CONF_ARGS="$XERCES_CONF_ARGS --enable-debug"
		XQILLA_CONF_ARGS="$XQILLA_CONF_ARGS --enable-debug"
		DEBUG_ENABLED="yes"
		;;
	--disable-rpath)
		#DB_CONF_ARGS="$DB_CONF_ARGS --disable-rpath"
		DBXML_CONF_ARGS="$DBXML_CONF_ARGS --disable-rpath"
		#XERCES_CONF_ARGS="$XERCES_CONF_ARGS"
		XQILLA_CONF_ARGS="$XQILLA_CONF_ARGS --disable-rpath"
		;;
	--enable-dependencies)
		DBXML_CONF_ARGS="$DBXML_CONF_ARGS --enable-dependency-tracking"
		XQILLA_CONF_ARGS="$XQILLA_CONF_ARGS --enable-dependency-tracking"
		;;
	--enable-java)
		DB_CONF_ARGS="$DB_CONF_ARGS $option"
		DBXML_CONF_ARGS="$DBXML_CONF_ARGS $option"
		JAVA_ENABLED="yes"
		;;
	--with-tcl=*)
		DB_CONF_ARGS="$DB_CONF_ARGS $option"
		DBXML_CONF_ARGS="$DBXML_CONF_ARGS $option"
		TCL_ENABLED="yes"
		;;
	--with-junit=*)
		DBXML_CONF_ARGS="$DBXML_CONF_ARGS $option"
		;;
        --enable-perl)                  BUILD_PERL="yes" ;;
        --perl-installdir=*)            PERL_INSTALLDIR="$optarg" ;;
	--with-dbxml=*)			DBXML_DIR="$optarg" ;;
	--with-berkeleydb=*)		DB_DIR="$optarg" ;;
	--with-xqilla=*)		XQILLA_DIR="$optarg" ;;
	--with-xerces=*)		XERCES_DIR="$optarg" ;;

	--with-dbxml-conf=*)		DBXML_CONF_XARGS="$DBXML_CONF_XARGS $optarg" ;;
	--with-berkeleydb-conf=*)	DB_CONF_XARGS="$optarg" ;;
	--with-xqilla-conf=*)		XQILLA_CONF_XARGS="$XQILLA_CONF_XARGS $optarg" ;;
	--with-xerces-conf=*)		XERCES_CONF_XARGS="$XERCES_CONF_XARGS $optarg" ;;

	--with-dbxml-prefix=*)		DBXML_INSTALL_DIR="$optarg" ;;
	--with-berkeleydb-prefix=*)	DB_INSTALL_DIR="$optarg" ;;
	--with-xqilla-prefix=*)		XQILLA_INSTALL_DIR="$optarg" ;;
	--with-xerces-prefix=*)		XERCES_INSTALL_DIR="$optarg" ;;

	--with-configure-env=*)		CONF_ENV="$CONF_ENV $optarg" ;;

	--build-one=*)
		if [ $BUILD_ONE = "no" ]; then
		    BUILD_ONE="$optarg"
		    case $BUILD_ONE in
			berkeleydb|xerces|xqilla|dbxml|perl) ;;
			*)
			    echo "$0: error: $BUILD_ONE is not a valid"
			    echo "library name for --build-one." 1>&2
			    exit 1
			    ;;
		    esac
		else
		    echo "$O: error: --build-one already specified" 1>&2
		    exit 1
	        fi
		;;

	-h|--help|-?)		usage ; exit 0 ;;

	*)	echo "$0: error: $option invalid option." 1>&2
		echo "Run with --help to see usage." 1>&2
		exit 1
		;;
	esac
done

# validate configuration state
if [ $TEST_ENABLED = "yes" ]; then
    if [ $TCL_ENABLED = "no" -a $JAVA_ENABLED = "no" ]; then
	echo "$0: error: --enable-test requires either --with-tcl or --enable-java" 1>&2
        exit 1
    fi
    if [ $TCL_ENABLED = "yes" ]; then
	DB_CONF_ARGS="$DB_CONF_ARGS --enable-test --enable-tcl"
    fi
fi

# Fix directories to be absolute
TOP=`pwd`
mkdir -p "$DBXML_INSTALL_DIR"
mkdir -p "$DBXML_BUILD_DIR"
DBXML_INSTALL_DIR=`cd $DBXML_INSTALL_DIR && pwd` || exit $?
mkdir -p "$DB_INSTALL_DIR"
DB_INSTALL_DIR=`cd $DB_INSTALL_DIR && pwd` || exit $?
mkdir -p "$XQILLA_INSTALL_DIR"
mkdir -p "$XQILLA_BUILD_DIR"
XQILLA_INSTALL_DIR=`cd $XQILLA_INSTALL_DIR && pwd` || exit $?
mkdir -p "$XERCES_INSTALL_DIR"
XERCES_INSTALL_DIR=`cd $XERCES_INSTALL_DIR && pwd` || exit $?

export CC
export CXX
export MAKE

# Do the work.  Order matters:
#   Berkeley DB
#   Xerces
#   XQilla
#   Berkeley DB XML

# deal with clean targets first
if [ $DO_CLEAN != "no" ]; then
    echo "Performing clean: $DO_CLEAN"
    # DB
    if [ $BUILD_ONE = "no" -o $BUILD_ONE = "berkeleydb" ]; then
	cd $DB_DIR/build_unix
	$MAKE $DO_CLEAN
	cd $TOP
    fi
    # Xerces
    if [ $BUILD_ONE = "no" -o $BUILD_ONE = "xerces" ]; then
	cd $XERCES_DIR
	$MAKE $DO_CLEAN
	cd $TOP
    fi
    # XQilla
    if [ $BUILD_ONE = "no" -o $BUILD_ONE = "xqilla" ]; then
	cd $XQILLA_BUILD_DIR
	$MAKE $DO_CLEAN
	cd $TOP
    fi
    # DBXML
    if [ $BUILD_ONE = "no" -o $BUILD_ONE = "dbxml" ]; then
	cd $DBXML_BUILD_DIR
	$MAKE $DO_CLEAN
    fi
    # Perl
    if [ $BUILD_ONE = "no" -o $BUILD_ONE = "perl" ]; then
	cd $DBXML_DIR/src/perl
	$MAKE $DO_CLEAN
    fi
    exit 0
fi

# Berkeley DB
if [ $BUILD_ONE = "no" -o $BUILD_ONE = "berkeleydb" ]; then
    echo Start DB build: `date`
    cd $DB_DIR/build_unix
    if [ $DO_CONFIGURE = "yes" ]; then
	echo Configuring Berkeley DB
	eval $CONF_ENV ../dist/configure --prefix=$DB_INSTALL_DIR --enable-cxx \
	    $DB_CONF_ARGS $DB_CONF_XARGS || exit $?
    fi

    echo Building Berkeley DB
    $MAKE || exit $?

    if [ $DO_INSTALL = "yes" ]; then
	echo Installing Berkeley DB
    # this conditional code is to avoid make install failures for
    # Berkeley DB when the distribution does not have documentation.
    # DB documentation is included in DB XML documentation.
	DOCLIST="api_reference articles collections csharp gsg gsg_db_rep gsg_txn index.html java license programmer_reference porting"
	if [ -d ../docs ]; then
            rmdocs="no"
            cd ../docs
            touch $DOCLIST 2>/dev/null
            cd ../build_unix
	else
            mkdir ../docs
            cd ../docs
            touch $DOCLIST
            rmdocs="yes"
            cd ../build_unix
	fi
	$MAKE install || exit $?
	# do not install DB docs
        rm -rf $DB_INSTALL_DIR/docs
	if [ $rmdocs = "yes" ]; then
            rm -rf ../docs
	fi

    fi
    cd $TOP
    echo End DB build: `date`
fi

echo "********************************"
echo "********************************"

XERCESCROOT=`cd $XERCES_DIR && pwd`
export XERCESCROOT

# Xerces
# change args, depending on platform
if [ $BUILD_ONE = "no" -o $BUILD_ONE = "xerces" ]; then
    echo Start Xerces build: `date`
    cd $XERCES_DIR
    if [ $DO_CONFIGURE = "yes" ]; then
	echo Configuring Xerces
 	eval $CONF_ENV ./configure --prefix=$XERCES_INSTALL_DIR \
	$XERCES_CONF_ARGS $XERCES_CONF_XARGS || exit $?
    fi

    echo Building Xerces
    # only build source/library tree
    cd src
    $MAKE || exit $?
    cd ..

    if [ $DO_INSTALL = "yes" ]; then
	echo Installing Xerces
    # remove symlinks that cause make install to fail if re-run
	for i in $XERCES_TO_REMOVE
	do
	    rm -f $XERCES_INSTALL_DIR/lib/$i
	done
    # only install source tree, not samples
	cd src
	$MAKE install || exit $?
	cd ..
    # remove unused xerces library.  Cannot suppress build, but can remove it.
	for i in $XERCES_DEPDOM
	  do
	  rm -f $XERCES_INSTALL_DIR/lib/$i
	done
    fi
    cd $TOP
    echo End Xerces build: `date`
fi

echo "********************************"
echo "********************************"

# XQilla
# change args, depending on platform/config
# XQilla depends on header files internal to Xerces, and
# requires --with-xerces to point to the source, rather
# than an install tree.
if [ $BUILD_ONE = "no" -o $BUILD_ONE = "xqilla" ]; then
    echo Start XQilla build: `date`
    cd $XQILLA_BUILD_DIR
    if [ $DO_CONFIGURE = "yes" ]; then
	echo Configuring XQilla
	eval $CONF_ENV ../configure --prefix=$XQILLA_INSTALL_DIR \
	    --with-xerces=$XERCESCROOT \
	    $XQILLA_CONF_ARGS $XQILLA_CONF_XARGS || exit $?
    fi

    echo Building XQilla
    $MAKE || exit $?

    if [ $DO_INSTALL = "yes" ]; then
	echo Installing XQilla
	$MAKE install || exit $?
    fi

    cd $TOP
    echo End XQilla build: `date`
fi

echo "********************************"
echo "********************************"

# DB XML
if [ $BUILD_ONE = "no" -o $BUILD_ONE = "dbxml" ]; then
    echo Start DB XML build: `date`
    cd $DBXML_BUILD_DIR
    if [ $DO_CONFIGURE = "yes" ]; then
        echo Configuring DB XML
         # Note: passing ../../$DB_DIR for --with-dbbuild will not work
         # if DB_DIR is absolute.  This is only used for test configuration,
         # and failure to find the directory is not fatal.
         eval $CONF_ENV ../configure --prefix=$DBXML_INSTALL_DIR \
             --with-xqilla=$XQILLA_INSTALL_DIR \
             --with-xerces=$XERCES_INSTALL_DIR \
             --with-berkeleydb=$DB_INSTALL_DIR \
             --with-dbbuild=../../$DB_DIR $DBXML_CONF_ARGS \
             $DBXML_CONF_XARGS || exit $?
    fi
    echo Building DB XML
    $MAKE || exit $?
    if [ $DO_INSTALL = "yes" ]; then
	echo Installing BDB XML
	$MAKE install || exit $?
    fi

    cd $TOP
    echo End DB XML build: `date`
fi

# Perl
if [ $BUILD_PERL = "yes" ]; then
if [ $BUILD_ONE = "no" -o $BUILD_ONE = "perl" ]; then
    echo Start Perl build: `date`
    echo "If Perl build fails, see $DBXML_DIR/src/perl/README"
    echo "for configuration and build instructions"
    PERL_INSTALL=""
    if [ "$PERL_INSTALLDIR" != "" ]; then
	PERL_INSTALL="PREFIX=$PERL_INSTALLDIR"
	echo "Perl PREFIX is $PERL_INSTALLDIR"
    fi
    cd $DBXML_DIR/src/perl
    if [ $DO_CONFIGURE = "yes" ]; then
	$PERL Makefile.PL $PERL_INSTALL
    fi
    echo Building Perl
    $MAKE || exit $?
    echo Testing Perl
    $MAKE test || exit $?
    if [ $DO_INSTALL = "yes" ]; then
	echo Installing Perl
	$MAKE install || exit $?
    fi

    cd $TOP
    echo End Perl build: `date`
fi
fi
