#!/usr/bin/env python
#----------------------------------------------------------------------
# Setup script for the bsddb3 package

import os
import re
import sys
import glob

try :
  from setuptools import setup, Extension
except :
  from distutils.core import setup, Extension

from distutils.dep_util import newer
import distutils.ccompiler

# read the module version number out of the .c file
VERSION = None
_ver_re = re.compile('^#\s*define\s+PY_BSDDB_VERSION\s+"(\d+\.\d+\.\d+.*)"')
try:
    _srcFile = open('Modules/bsddb.h', 'r')
except IOError:
    print "Could not open module source to read the version number."
    raise
for line in _srcFile.readlines():
    m = _ver_re.match(line)
    if m:
        VERSION = m.group(1)
        continue
del _srcFile
del _ver_re
del m
if not VERSION:
    raise RuntimeError, "could not find PY_BSDDB_VERSION in Modules/bsddb.h"

#----------------------------------------------------------------------

debug = '--debug' in sys.argv or '-g' in sys.argv

lflags_arg = []


if os.name == 'posix':
    # Allow setting the DB dir and additional link flags either in
    # the environment or on the command line.
    # First check the environment...
    BERKELEYDB_INCDIR = os.environ.get('BERKELEYDB_INCDIR', '')
    BERKELEYDB_LIBDIR = os.environ.get('BERKELEYDB_LIBDIR', '')
    BERKELEYDB_DIR = os.environ.get('BERKELEYDB_DIR', '')
    LFLAGS = os.environ.get('LFLAGS', [])
    LIBS = os.environ.get('LIBS', [])

    # ...then the command line.
    # Handle --berkeley-db=[PATH] and --lflags=[FLAGS]
    args = sys.argv[:]
    for arg in args:
        if arg.startswith('--berkeley-db-incdir='):
            BERKELEYDB_INCDIR = arg.split('=')[1]
            sys.argv.remove(arg)
        if arg.startswith('--berkeley-db-libdir='):
            BERKELEYDB_LIBDIR = arg.split('=')[1]
            sys.argv.remove(arg)
        if arg.startswith('--berkeley-db='):
            BERKELEYDB_DIR = arg.split('=')[1]
            sys.argv.remove(arg)
        elif arg.startswith('--lflags='):
            LFLAGS = arg.split('=')[1].split()
            sys.argv.remove(arg)
        elif arg.startswith('--libs='):
            LIBS = arg.split('=')[1].split()
            sys.argv.remove(arg)

    if LFLAGS or LIBS:
        lflags_arg = LFLAGS + LIBS

    # If we were not told where it is, go looking for it.
    dblib = 'db'
    incdir = libdir = None
    if not BERKELEYDB_DIR and not BERKELEYDB_LIBDIR and not BERKELEYDB_INCDIR:
        # NOTE: when updating these, also change the tuples in the for loops below
        max_db_ver = (4, 8)
        min_db_ver = (4, 1)

        # construct a list of paths to look for the header file in on
        # top of the normal inc_dirs.
        db_inc_paths = [
            '/usr/include/db4',
            '/usr/local/include/db4',
            '/opt/sfw/include/db4',
            '/sw/include/db4',
            '/usr/include/db3',
            '/usr/local/include/db3',
            '/opt/sfw/include/db3',
            '/sw/include/db3',
        ]
        # 4.x minor number specific paths
        for x in range(max_db_ver[1]+1):
            db_inc_paths.append('/usr/include/db4%d' % x)
            db_inc_paths.append('/usr/local/BerkeleyDB.4.%d/include' % x)
            db_inc_paths.append('/usr/local/include/db4%d' % x)
            db_inc_paths.append('/pkg/db-4.%d/include' % x)
            db_inc_paths.append('/opt/db-4.%d/include' % x)
        # 3.x minor number specific paths
        for x in (2,3):
            db_inc_paths.append('/usr/include/db3%d' % x)
            db_inc_paths.append('/usr/local/BerkeleyDB.3.%d/include' % x)
            db_inc_paths.append('/usr/local/include/db3%d' % x)
            db_inc_paths.append('/pkg/db-3.%d/include' % x)
            db_inc_paths.append('/opt/db-3.%d/include' % x)

        db_ver_inc_map = {}

        class db_found(Exception): pass
        try:
            # this CCompiler object is only used to locate include files
            compiler = distutils.ccompiler.new_compiler()

            lib_dirs = compiler.library_dirs + [
                '/lib64', '/usr/lib64',
                '/lib', '/usr/lib',
            ]
            inc_dirs = compiler.include_dirs + ['/usr/include']

            # See whether there is a Oracle or Sleepycat header in the standard
            # search path.
            for d in inc_dirs + db_inc_paths:
                f = os.path.join(d, "db.h")
                if debug: print "db: looking for db.h in", f
                if os.path.exists(f):
                    f = open(f).read()
                    m = re.search(r"#define\WDB_VERSION_MAJOR\W(\d+)", f)
                    if m:
                        db_major = int(m.group(1))
                        m = re.search(r"#define\WDB_VERSION_MINOR\W(\d+)", f)
                        db_minor = int(m.group(1))
                        db_ver = (db_major, db_minor)

                        if ( (not db_ver_inc_map.has_key(db_ver)) and
                           (db_ver <= max_db_ver and db_ver >= min_db_ver) ):
                            # save the include directory with the db.h version
                            # (first occurrance only)
                            db_ver_inc_map[db_ver] = d
                            if debug: print "db.h: found", db_ver, "in", d
                        else:
                            # we already found a header for this library version
                            if debug: print "db.h: ignoring", d
                    else:
                        # ignore this header, it didn't contain a version number
                        if debug: print "db.h: unsupported version", db_ver, "in", d

            db_found_vers = db_ver_inc_map.keys()
            db_found_vers.sort()

            while db_found_vers:
                db_ver = db_found_vers.pop()
                db_incdir = db_ver_inc_map[db_ver]

                # check lib directories parallel to the location of the header
                db_dirs_to_check = [
                    os.path.join(db_incdir, '..', 'lib64'),
                    os.path.join(db_incdir, '..', 'lib'),
                    os.path.join(db_incdir, '..', '..', 'lib64'),
                    os.path.join(db_incdir, '..', '..', 'lib'),
                ]
                db_dirs_to_check = filter(os.path.isdir, db_dirs_to_check)

                # Look for a version specific db-X.Y before an ambiguoius dbX
                # XXX should we -ever- look for a dbX name?  Do any
                # systems really not name their library by version and
                # symlink to more general names?
                for dblib in (('db-%d.%d' % db_ver), ('db%d' % db_ver[0])):
                    dblib_file = compiler.find_library_file(
                                    db_dirs_to_check + lib_dirs, dblib )
                    if dblib_file:
                        db_libdir = os.path.abspath(os.path.dirname(dblib_file))
                        raise db_found
                    else:
                        if debug: print "db lib: ", dblib, "not found"
        except db_found:
            print "Found Berkeley DB %d.%d installation." % db_ver
            print "  include files in", db_incdir
            print "  library files in", db_libdir
            print "  library name is lib"+dblib
            if debug: print "db: lib dir", db_libdir, "inc dir", db_incdir

            incdir  = db_incdir
            libdir  = db_libdir
        else:
            # this means Berkeley DB could not be found
            pass

    if BERKELEYDB_LIBDIR or BERKELEYDB_INCDIR:
        libdir = BERKELEYDB_LIBDIR or None
        incdir = BERKELEYDB_INCDIR or None

    if not BERKELEYDB_DIR and not incdir and not libdir:
        print "Can't find a local Berkeley DB installation."
        print "(suggestion: try the --berkeley-db=/path/to/bsddb option)"
        sys.exit(1)

    # figure out from the base setting where the lib and .h are
    if not incdir:
        incdir = os.path.join(BERKELEYDB_DIR, 'include')
    if not libdir:
        libdir = os.path.join(BERKELEYDB_DIR, 'lib')
    if not '-ldb' in LIBS:
        libname = [dblib]
    else:
        if debug: print "LIBS already contains '-ldb' not adding our own", "'-l"+dblib+"'"
        libname = []
    utils = []

    # Test if the old bsddb is built-in
    static = 0
    try:
        import bsddb
        if str(bsddb).find('built-in') >= 0:
            static = 1
    except ImportError:
        pass

    # On Un*x, double check that no other built-in module pulls libdb in as a
    # side-effect. TBD: how/what to do on other platforms?
    fp = os.popen('ldd %s 2>&1' % sys.executable)
    results = fp.read()
    status = fp.close()
    if not status and results.find('libdb.') >= 0:
        static = 1

    if static:
        print """\
\aWARNING:
\tIt appears that the old bsddb module is staticly linked in the
\tPython executable. This will cause various random problems for
\tbsddb3, up to and including segfaults. Please rebuild your
\tPython either with bsddb disabled, or with it built as a shared
\tdynamic extension. Watch out for other modules (e.g. dbm) that create
\tdependencies in the python executable to libdb as a side effect."""
        st = raw_input("Build anyway? (yes/[no]) ")
        if st != "yes":
            sys.exit(1)


elif os.name == 'nt':

    # The default build of Berkeley DB for windows just leaves
    # everything in the build dirs in the db source tree. That means
    # that we either have to hunt around to find it, (which would be
    # even more difficult than the mess above for Unix...) or we make
    # the builder specify everything here. Compounding the problem is
    # version numbers in default path names, and different library
    # names for debug/release or dll/static.
    #
    # So to make things easier, I'm just going to exepect that the DB stuff
    # has been moved to the ./db directory. There's an updatedb.bat file to
    # help.
    #
    # You'll need to edit the project file that comes with Berkeley DB so it
    # uses "Multithreaded DLL" and "Debug Multithreaded DLL"  (/MD and /MDd)
    # settings as appropriate to build .lib file (the db_static project).

    incdir = 'db/include'
    libdir = 'db/lib'

    # read db.h to figure out what version of Berkeley DB this is
    ver = None
    db_h_lines = open(os.path.join(incdir, 'db.h'), 'r').readlines()
    db_ver_re = re.compile(
        r'^#define\s+DB_VERSION_STRING\s.*Berkeley DB (\d+\.\d+).*')
    for line in db_h_lines:
        match = db_ver_re.match(line)
        if not match:
            continue
        fullverstr = match.group(1)
        ver = fullverstr[0] + fullverstr[2]   # 31 == 3.1, 32 == 3.2, etc.
    assert ver in ('41', '42', '43', '44', '45', '46', '47', '48'), (
        "pybsddb untested with this Berkeley DB version", ver)
    print 'Detected Berkeley DB version', ver, 'from db.h'

    if debug:
        libname = ['libdb%ssd' % ver]     # Debug, static
    else:
        libname = ['libdb%ss' % ver]      # Release, static
    utils = [("bsddb3/utils",
              ["db/bin/db_archive.exe",
               "db/bin/db_checkpoint.exe",
               "db/bin/db_deadlock.exe",
               "db/bin/db_dump.exe",
               "db/bin/db_load.exe",
               "db/bin/db_printlog.exe",
               "db/bin/db_recover.exe",
               "db/bin/db_stat.exe",
               "db/bin/db_upgrade.exe",
               "db/bin/db_verify.exe",
               "db/bin/libdb%s.dll" % ver,
               ]),
             ("bsddb3/test", glob.glob("test/*.py"))
             ]

version_suffix = ""
if sys.version_info[0] > 2 :
    version_suffix = "3"

# do the actual build, install, whatever...
setup(name = 'bsddb3',
      version = VERSION,
      description = 'Python interface for Berkeley DB',
      long_description = """\
This module provides a nearly complete wrapping
of the Oracle/Sleepycat C API for the Database
Environment, Database, Cursor, Sequence and Transaction
objects, and each of these is exposed as a Python
type in the bsddb3.db module. The database objects
can use various access methods: btree, hash, recno,
and queue. Complete support of Berkeley DB distributed
transactions. Complete support for Berkeley DB
Replication Manager. Complete support for Berkeley DB
Base Replication. Support for RPC.

Please see the documents in the docs
directory of the source distribution or at the
website for more details on the types and methods
provided. The goal is to mirror most of the real
Berkeley DB API so fall back to the Oracle Berkeley DB
documentation as appropriate.

`Homepage <http://www.jcea.es/programacion/
pybsddb.htm>`__ --
`Documentation <http://www.jcea.es/programacion/
pybsddb_doc/>`__ --
`Mailing List <http://mailman.jcea.es/listinfo/pybsddb>`__
""",

      author = 'Jesus Cea, Robin Dunn, Gregory P. Smith, Andrew Kuchling, Barry Warsaw',
      author_email = 'pybsddb@jcea.es',
      url = 'http://www.jcea.es/programacion/pybsddb.htm',
      license = "MIT License",

      packages = ['bsddb3', 'bsddb3/tests'],
      package_dir = {'bsddb3': 'Lib%s/bsddb' %version_suffix,
                     'bsddb3/tests': 'Lib%s/bsddb/test' %version_suffix},
      ext_modules = [Extension('bsddb3._pybsddb',
                               sources = ['Modules/_bsddb.c'],
                               depends = ['Modules/bsddb.h'],
                               include_dirs = [ incdir ],
                               define_macros = [('PYBSDDB_STANDALONE', 1)],
                               library_dirs = [ libdir ],
                               runtime_library_dirs = [ libdir ],
                               libraries = libname,
                               extra_link_args = lflags_arg,
                               )],
      headers = ['Modules/bsddb.h'],
      data_files = utils,
      classifiers = [
                    'License :: OSI Approved :: MIT License',
                    'Development Status :: 5 - Production/Stable',
                    'Intended Audience :: Developers',
                    'Intended Audience :: Information Technology',
                    'Natural Language :: English',
                    'Natural Language :: Spanish',
                    'Operating System :: OS Independent',
                    'Programming Language :: Python',
                    'Topic :: Database',
                    'Topic :: Software Development',
                    'Topic :: System :: Clustering',
                    'Programming Language :: Python :: 2',
                    'Programming Language :: Python :: 2.3',
                    'Programming Language :: Python :: 2.4',
                    'Programming Language :: Python :: 2.5',
                    'Programming Language :: Python :: 2.6',
                    'Programming Language :: Python :: 2.7',
                    'Programming Language :: Python :: 3',
                    'Programming Language :: Python :: 3.0',
                    'Programming Language :: Python :: 3.1',
                    'Programming Language :: Python :: 3.2',
                    ],
      )
