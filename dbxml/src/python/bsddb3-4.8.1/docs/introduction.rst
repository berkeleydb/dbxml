.. $Id: introduction.rst 486 2008-05-31 04:34:34Z jcea $

==============================================
Berkeley DB 4.x Python Extension Package
==============================================

Introduction
------------

This is a simple bit of documentation for the bsddb3.db Python extension
module which wraps the Berkeley DB 4.x C library. The extension
module is located in a Python package along with a few pure python
modules.

It is expected that this module will be used in the following general
ways by different programmers in different situations. The goals of
this module are to allow all of these methods without making things too
complex for the simple cases, and without leaving out funtionality
needed by the complex cases.


1. **Backwards compatibility:** It is desirable for this package to be a
   near drop-in replacement for the bsddb module shipped with Python
   which is designed to wrap either DB 1.85, or the 1.85 compatibility
   interface. This means that there will need to be equivalent object
   creation functions available, (btopen(), hashopen(), and rnopen())
   and the objects returned will need to have the same or at least
   similar methods available, (specifically, first(), last(), next(),
   and prev() will need to be available without the user needing to
   explicitly use a cursor.)  All of these have been implemented in
   Python code in the bsddb3.__init__.py module.

2. **Simple persistent dictionary:** One small step beyond the above.
   The programmer may be aware of and use the new DB object type
   directly, but only needs it from a single process and thread. The
   programmer should not have to be bothered with using a DBEnv, and the
   DB object should behave as much like a dictionary as possible.

3. **Concurrent access dictionaries:** This refers to the ability to
   simultaneously have one writer and multiple readers of a DB (either
   in multiple threads or processes) and is implemented simply by
   creating a DBEnv with certain flags. No extra work is required to
   allow this access mode in bsddb3.

4. **Advanced transactional data store:** This mode of use is where the
   full capabilities of the Berkeley DB library are called into action.
   The programmer will probably not use the dictionary access methods as
   much as the regular methods of the DB object, so he can pass
   transaction objects to the methods. Again, most of this advanced
   functionality is activated simply by opening a DBEnv with the proper
   flags, and also by using transactions and being aware of and reacting
   to deadlock exceptions, etc.

Types Provided
--------------

The bsddb3.db extension module provides the following object types:

- **DB:** The basic database object, capable of Hash, BTree, Recno, and
  Queue access methods.

- **DBEnv:** Provides a Database Environment for more advanced database
  use. Apps using transactions, logging, concurrent access, etc. will
  need to have an environment object.

- **DBCursor:** A pointer-like object used to traverse a database.

- **DBTxn:** A database transaction. Allows for multi-file commit, abort
  and checkpoint of database modifications.

- **DBLock:** An opaque handle for a lock. See DBEnv.lock_get() and
  DBEnv.lock_put(). Locks are not necessarily associated with anything
  in the database, but can be used for any syncronization task across
  all threads and processes that have the DBEnv open.

- **DBSequence:** Sequences provide an arbitrary number of persistent
  objects that return an increasing or decreasing sequence of integers.
  Opening a sequence handle associates it with a record in a database.

Exceptions Provided
-------------------

The Berkeley DB C API uses function return codes to signal various
errors. The bsddb3.db module checks for these error codes and turns them
into Python exceptions, allowing you to use familiar try:... except:...
constructs and not have to bother with checking every method's return
value.

Each of the error codes is turned into an exception specific to that
error code, as outlined in the table below. If you are using the C API
documentation then it is very easy to map the error return codes
specified there to the name of the Python exception that will be raised.
Simply refer to the table below.

Each exception derives from the DBError exception class so if you just
want to catch generic errors you can use DBError to do it. Since
DBNotFoundError is raised when a given key is not found in the database,
DBNotFoundError also derives from the standard KeyError exception to
help make a DB look and act like a dictionary.

When any of these exceptions is raised, the associated value is a tuple
containing an integer representing the error code and a string for the
error message itself.

    +---------------------------+-------------------------------------------+
    | **DBError**               | Base class, all others derive from this   |
    +---------------------------+-------------------------------------------+
    | **DBIncompleteError**     | DB_INCOMPLETE                             |
    +---------------------------+-------------------------------------------+
    | **DBKeyEmptyError**       | DB_KEYEMPTY                               |
    +---------------------------+-------------------------------------------+
    | **DBKeyExistError**       | DB_KEYEXIST                               |
    +---------------------------+-------------------------------------------+
    | **DBLockDeadlockError**   | DB_LOCK_DEADLOCK                          |
    +---------------------------+-------------------------------------------+
    | **DBLockNotGrantedError** | DB_LOCK_NOTGRANTED                        |
    +---------------------------+-------------------------------------------+
    | **DBNotFoundError**       | DB_NOTFOUND (also derives from KeyError)  |
    +---------------------------+-------------------------------------------+
    | **DBOldVersionError**     | DB_OLD_VERSION                            |
    +---------------------------+-------------------------------------------+
    | **DBRunRecoveryError**    | DB_RUNRECOVERY                            |
    +---------------------------+-------------------------------------------+
    | **DBVerifyBadError**      | DB_VERIFY_BAD                             |
    +---------------------------+-------------------------------------------+
    | **DBNoServerError**       | DB_NOSERVER                               |
    +---------------------------+-------------------------------------------+
    | **DBNoServerHomeError**   | DB_NOSERVER_HOME                          |
    +---------------------------+-------------------------------------------+
    | **DBNoServerIDError**     | DB_NOSERVER_ID                            |
    +---------------------------+-------------------------------------------+
    | **DBInvalidArgError**     | EINVAL                                    |
    +---------------------------+-------------------------------------------+
    | **DBAccessError**         | EACCES                                    |
    +---------------------------+-------------------------------------------+
    | **DBNoSpaceError**        | ENOSPC                                    |
    +---------------------------+-------------------------------------------+
    | **DBNoMemoryError**       | ENOMEM                                    |
    +---------------------------+-------------------------------------------+
    | **DBAgainError**          | EAGAIN                                    |
    +---------------------------+-------------------------------------------+
    | **DBBusyError**           | EBUSY                                     |
    +---------------------------+-------------------------------------------+
    | **DBFileExistsError**     | EEXIST                                    |
    +---------------------------+-------------------------------------------+
    | **DBNoSuchFileError**     | ENOENT                                    |
    +---------------------------+-------------------------------------------+
    | **DBPermissionsError**    | EPERM                                     |
    +---------------------------+-------------------------------------------+

Other Package Modules
---------------------

- **dbshelve.py:** This is an implementation of the standard Python
  shelve concept for storing objects that uses bsddb3 specifically, and
  also exposes some of the more advanced methods and capabilities of the
  underlying DB.

- **dbtables.py:** This is a module by Gregory Smith that implements a
  simplistic table structure on top of a DB.

- **dbutils.py:** A catch-all for python code that is generally useful
  when working with DB's

- **dbobj.py:** Contains subclassable versions of DB and DBEnv.

- **dbrecio.py:** Contains the DBRecIO class that can be used to do
  partial reads and writes from a DB record using a file-like interface.
  Contributed by Itamar Shtull-Trauring.

Testing
-------

A full unit test suite is being developed to exercise the various object
types, their methods and the various usage modes described in the
introduction. `PyUnit <http://pyunit.sourceforge.net/>`__ is used and
the tests are structured such that they can be run unattended and
automated. There are currently almost 300 test cases!  (March 2008)

Reference
---------

See the C language API `online documentation
<http://www.oracle.com/technology/documentation/berkeley-db/
db/index.html>`__ on Oracle's website for more details of the
functionality of each of these methods. The names of all the Python
methods should be the same or similar to the names in the C API.

**NOTE:** All the methods shown below having more than one keyword
argument are actually implemented using keyword argument parsing, so you
can use keywords to provide optional parameters as desired. Those that
have only a single optional argument are implemented without keyword
parsing to help keep the implementation simple. If this is too confusing
let me know and I'll think about using keywords for everything.

