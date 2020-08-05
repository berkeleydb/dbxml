.. $Id: contents.rst 697 2009-06-10 11:01:12Z jcea $

=============================================
 Python Bindings for Berkeley DB 4.1 thru 4.8
=============================================

Introduction
------------

.. _Berkeley DB: http://www.oracle.com/database/berkeley-db.html

This handcrafted package contains Python wrappers for `Berkeley DB`_,
the Open Source embedded database system. Berkeley DB is a programmatic
toolkit that provides high-performance built-in database support for
desktop and server applications.

The Berkeley DB access methods include B+tree, Extended Linear Hashing,
Fixed and Variable-length records, and Queues. Berkeley DB provides full
transactional support, database recovery, online backups, multi-threaded
and multi-process access, etc.

The Python wrappers allow you to store Python string objects of any
length, keyed either by strings or integers depending on the database
access method. With the use of another module in the package standard
shelve-like functionality is provided allowing you to store any
picklable Python object! 

Documentation Index
-------------------

.. toctree::

  introduction.rst
  dbenv.rst
  db.rst
  dbcursor.rst
  dbtxn.rst
  dblock.rst
  dbsequence.rst
  history.rst

