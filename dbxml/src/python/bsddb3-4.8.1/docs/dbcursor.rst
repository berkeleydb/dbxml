.. $Id: dbcursor.rst 690 2009-04-28 14:03:38Z jcea $

========
DBCursor
========

DBCursor Methods
----------------

.. function:: close()

   Discards the cursor. If the cursor is created within a transaction
   then you *must* be sure to close the cursor before commiting the
   transaction.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_close.html>`__

.. function:: count(flags=0)

   Returns a count of the number of duplicate data items for the key
   referenced by the cursor.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_count.html>`__

.. function:: delete(flags=0)

   Deletes the key/data pair currently referenced by the cursor.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_del.html>`__

.. function:: dup(flags=0)

   Create a new cursor.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_del.html>`__

.. function:: set_priority(priority)

   Set the cache priority for pages referenced by the DBC handle.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_set_priority.html>`__

.. function:: get_priority()

   Returns the cache priority for pages referenced by the DBC handle.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_set_priority.html>`__

.. function:: put(key, data, flags=0, dlen=-1, doff=-1)

   Stores the key/data pair into the database. Partial data records can
   be written using dlen and doff.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_put.html>`__

.. function:: get(flags, dlen=-1, doff=-1)

   See get(key, data, flags, dlen=-1, doff=-1) below.

.. function:: get(key, flags, dlen=-1, doff=-1)

   See get(key, data, flags, dlen=-1, doff=-1) below.

.. function:: get(key, data, flags, dlen=-1, doff=-1)

   Retrieves key/data pairs from the database using the cursor. All the
   specific functionalities of the get method are actually provided by
   the various methods below, which are the preferred way to fetch data
   using the cursor. These generic interfaces are only provided as an
   inconvenience. Partial data records are returned if dlen and doff
   are used in this method and in many of the specific methods below.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_get.html>`__

.. function:: pget(flags, dlen=-1, doff=-1)

   See pget(key, data, flags, dlen=-1, doff=-1) below.

.. function:: pget(key, flags, dlen=-1, doff=-1)

   See pget(key, data, flags, dlen=-1, doff=-1) below.

.. function:: pget(key, data, flags, dlen=-1, doff=-1)

   Similar to the already described get(). This method is available only
   on secondary databases. It will return the primary key, given the
   secondary one, and associated data
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_get.html>`__

DBCursor Get Methods
--------------------

These DBCursor methods are all wrappers around the get() function in the
C API.

.. function:: current(flags=0, dlen=-1, doff=-1)

   Returns the key/data pair currently referenced by the cursor.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_get.html#DB_CURRENT>`__

.. function:: get_current_size()

   Returns length of the data for the current entry referenced by the
   cursor.

.. function:: first(flags=0, dlen=-1, doff=-1)

   Position the cursor to the first key/data pair and return it.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_get.html#DB_FIRST>`__

.. function:: last(flags=0, dlen=-1, doff=-1)

   Position the cursor to the last key/data pair and return it.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_get.html#DB_LAST>`__

.. function:: next(flags=0, dlen=-1, doff=-1)

   Position the cursor to the next key/data pair and return it.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_get.html#DB_NEXT>`__

.. function:: prev(flags=0, dlen=-1, doff=-1)

   Position the cursor to the previous key/data pair and return it.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_get.html#DB_PREV>`__

.. function:: consume(flags=0, dlen=-1, doff=-1)

   For a database with the Queue access method, returns the record
   number and data from the first available record and deletes it from
   the queue.

   *NOTE:* This method is deprecated in Berkeley DB version 3.2 in favor
   of the new consume method in the DB class.

.. function:: get_both(key, data, flags=0)

   Like set() but positions the cursor to the record matching both key
   and data. (An alias for this is set_both, which makes more sense to
   me...)
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_get.html#DB_GET_BOTH>`__

.. function:: get_recno()

   Return the record number associated with the cursor. The database
   must use the BTree access method and have been created with the
   DB_RECNUM flag.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_get.html#DB_GET_RECNO>`__

.. function:: join_item(flags=0)

   For cursors returned from the DB.join method, returns the combined
   key value from the joined cursors.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_get.html#DB_JOIN_ITEM>`__

.. function:: next_dup(flags=0, dlen=-1, doff=-1)

   If the next key/data pair of the database is a duplicate record for
   the current key/data pair, the cursor is moved to the next key/data
   pair of the database, and that pair is returned.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_get.html#DB_NEXT_DUP>`__

.. function:: next_nodup(flags=0, dlen=-1, doff=-1)

   The cursor is moved to the next non-duplicate key/data pair of the
   database, and that pair is returned.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_get.html#DB_NEXT_NODUP>`__

.. function:: prev_dup(flags=0, dlen=-1, doff=-1)

   If the previous key/data pair of the database is a duplicate data
   record for the current key/data pair, the cursor is moved to the
   previous key/data pair of the database, and that pair is returned. 
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_get.html#DB_PREV_DUP>`__

.. function:: prev_nodup(flags=0, dlen=-1, doff=-1)

   The cursor is moved to the previous non-duplicate key/data pair of
   the database, and that pair is returned.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_get.html#DB_PREV_NODUP>`__

.. function:: set(key, flags=0, dlen=-1, doff=-1)

   Move the cursor to the specified key in the database and return the
   key/data pair found there.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_get.html#DB_SET>`__

.. function:: set_range(key, flags=0, dlen=-1, doff=-1)

   Identical to set() except that in the case of the BTree access
   method, the returned key/data pair is the smallest key greater than
   or equal to the specified key (as determined by the comparison
   function), permitting partial key matches and range searches.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_get.html#DB_SET_RANGE>`__

.. function:: set_recno(recno, flags=0, dlen=-1, doff=-1)

   Move the cursor to the specific numbered record of the database, and
   return the associated key/data pair. The underlying database must be
   of type Btree and it must have been created with the DB_RECNUM flag.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_get.html#DB_SET_RECNO>`__

.. function:: set_both(key, data, flags=0)

   See get_both(). The only difference in behaviour can be disabled
   using set_get_returns_none(2).
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/dbc_get.html#DB_GET_BOTH>`__

