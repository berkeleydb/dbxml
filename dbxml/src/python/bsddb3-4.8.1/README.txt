THIS DOCUMENT NEEDS TO BE UPDATED. THE INFO IT CONTAINS 
IS STILL VALUABLE, SO USE YOUR BRAIN :-)


README.txt
bsddb3 v4.8.x
http://www.jcea.es/programacion/pybsddb.htm
2009-06-30
--------------------------------

This module is intended to be the definitive modern Berkeley DB
interface for Python.  It should be both backwards compatible with
the earlier (circa Python 1.5 to 2.2) "bsddb" module in addition to
providing access to the much more powerful interface found in
Berkeley DB 4.x.

This module provides a nearly complete wrapping of the Oracle C API
for the Database Environment, Database, Cursor, Transaction and Lock
objects.  Each of these is exposed as a Python Type in the
bsddb3.db module.  The database objects can use different access
methods: btree, hash, recno, and queue.  For the first time all of
these are fully supported in the Python wrappers.  Please see the
documents in the docs directory for more details on the types and
methods provided.

There is also a collection of test cases in the test directory that
can be used to validate the modules, as well as giving an idea of how
to use bsddb3.db.

Thanks to The Written Word (http://thewrittenword.com/), bsddb3 was
known to pass its unit tests on these platforms:

     HP-UX        10.20, 11.00
     Tru64 UNIX   4.0D, 5.0, 5.1
     IRIX         6.2, 6.5
     Solaris      2.5.1, 2.6, 7, 8, 9, 10
     AIX          4.3.2

In addition we've run it on a several varieties of Linux and of course
on Win32.

This wrapper should be compatible with Berkeley DB releases going back
to 4.1 up to and including DB 4.8.  It should also be compatible
with Python versions from 2.3 to Python 2.7, and Python 3.0 to 3.2.


Installation
------------

If you are on a Win32 system then you can just get the binary
installer and run it and then you'll be all set.  If you want to build
it yourself, you can follow the directions below, but see the comments
in setup.py first.

If you are on a Unix/Linux system then keep reading...

The Python Distutils are used to build and install bsddb3, so it is
fairly simple to get things ready to go.

0. Check the patches directory for any updates required for the
   version of the Berkeley DB library that you have.  If a patch(s)
   are indicated in the README then apply it to your Berkeley DB
   sources and rebuild it.

1. First, make sure that you have a Berkeley DB version >= 4.1 and
   no later than 4.8 and that it is built and installed.  Setup.py will
   detect a db3 or Berkeley DB directory under either /usr/local or /usr
   in that order; this will catch installations from RPMs and most hand
   installations under Unix.  If setup.py can't find your libdb then you
   can give it a hint either in the environment (using the BERKELEYDB_DIR
   environment variable) or on the command line by specifying the
   directory containing the include and lib directory.  For example:

            python setup.py --berkeley-db=/stuff/BerkeleyDB.4.8 build

   If your Berkeley DB was built as a shared library, and if that
   shared library is not on the runtime load path, then you can
   specify the additional linker flags needed to find the shared
   library on the command line as well.  For example:

           --lflags="-Xlinker -rpath -Xlinker /stuff/BerkeleyDB.4.8/lib"

   or perhaps just

           --lflags="-R /stuff/BerkeleyDB.4.8/lib"

   Check your compiler and linker documentation to be sure.

   It is also possible to specify linking against a different library
   with the --libs switch:

           --libs="-ldb3"
           --libs="-ldb3 -lnsl"

   On some OSes distutils may not always use the correct compiler or
   linker flags.  One user reported having to add the "-lpthread" option
   in order to link with Berkeley DB that was compiled with threads on AIX.


   NOTE: My recommendation is to not rely on pre-built versions of
   Berkeley DB since on some systems there can be several versions
   already installed and it's sometimes difficult to tell which you
   are linking with when building bsddb3.  Instead get the sources
   from http://www.sleepycat.com/ and build/install it yourself as a
   static library and let setup.py find it in /usr/local.  It's not
   such a big library that you have to worry about wasting space by
   not dynamically linking and doing it this way will possibly save
   you some gray hairs.



2. From the main bsddb3 distribution directory run this command, (plus
   any extra flags needed as discussed above):

        python setup.py build

   If your Python was built with the old bsddb module built-in
   (statically linked) then the setup script will complain and you'll
   have to type "yes" to build anything.  Please see the note below
   about remedying this.


3. To run the test suite, run the following command:

        python test.py

   If you would like to see some verbose output from the tests simply
   add a -v option.  A second -v provides even more verbose output.
   The test.py driver script provides lots of options for debugging,
   running individual tests, etc.  Do "python test.py -h" for details.


4. To install the extension module and the Python modules in the
   bsddb3 package, run this command as the root user:

        python setup.py install


That's it!


Developer Information
---------------------

Both the Python and C source code is now shared with the Python
project.  If you're checking the code out from Subversion, you'll need to run
the getsvn.sh script to get a copy of the code.  How you run this
depends on whether you have write access to the Python Subversion or not
(most people do not -- you would know if you do).

To check the code out, do the following:

    % ./getsvn.sh anon

This will do an anonymous checkout of the code.  To do an
authenticated (via ssh) checkout, do the following:

    % ./getsvn.sh

This will check out an extsrc directory and a bsddb directory.  There
is one caveat: you'll find that extsrc will become populated with
every C file in Python's Modules directory.  You should just ignore this.


-- Robin
robin@alldunn.com

-- [later updates by] Gregory P. Smith
greg@krypto.org

-- [even later updates by] Barry A. Warsaw
barry@python.org

Got questions? email pybsddb-users@lists.sf.net

