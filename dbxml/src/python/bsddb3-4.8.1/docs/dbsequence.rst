.. $Id: dbsequence.rst 648 2009-02-04 12:43:34Z jcea $

==========
DBSequence
==========

Sequences provide an arbitrary number of persistent objects that return
an increasing or decreasing sequence of integers. Opening a sequence
handle associates it with a record in a database. The handle can
maintain a cache of values from the database so that a database update
is not needed as the application allocates a value.

`More info...
<http://www.oracle.com/technology/documentation/berkeley-db/db/
ref/sequence/intro.html>`__

DBSequence Methods
------------------

.. function:: DBSequence(db, flags=0)

   Constructor.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/seq_class.html>`__

.. function:: open(key, txn=None, flags=0)

   Opens the sequence represented by the key.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/seq_open.html>`__

.. function:: close(flags=0)

   Close a DBSequence handle.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/seq_close.html>`__

.. function:: initial_value(value)

   Set the initial value for a sequence. This call is only effective
   when the sequence is being created.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/seq_initial_value.html>`__

.. function:: get(delta=1, txn=None, flags=0)

   Returns the next available element in the sequence and changes the
   sequence value by delta.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/seq_get.html>`__

.. function:: get_dbp()

   Returns the DB object associated to the DBSequence.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/seq_open.html>`__

.. function:: get_key()

   Returns the key for the sequence.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/seq_open.html>`__

.. function:: remove(txn=None, flags=0)

   Removes the sequence from the database. This method should not be
   called if there are other open handles on this sequence.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/seq_remove.html>`__

.. function:: get_cachesize()

   Returns the current cache size.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/seq_set_cachesize.html>`__

.. function:: set_cachesize(size)

   Configure the number of elements cached by a sequence handle.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/seq_set_cachesize.html>`__

.. function:: get_flags()

   Returns the current flags.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/seq_set_flags.html>`__

.. function:: set_flags(flags)

   Configure a sequence.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/seq_set_flags.html>`__

.. function:: stat(flags=0)

   Returns a dictionary of sequence statistics with the following keys:

     +------------+----------------------------------------------+
     | wait       | The number of times a thread of control was  |
     |            | forced to wait on the handle mutex.          |
     +------------+----------------------------------------------+         
     | nowait     | The number of times that a thread            |
     |            | of control was able to obtain handle mutex   |
     |            | without waiting.                             |
     +------------+----------------------------------------------+           
     | current    | The current value of the sequence            |
     |            | in the database.                             |
     +------------+----------------------------------------------+            
     | value      | The current cached value of the sequence.    |
     +------------+----------------------------------------------+
     | last_value | The last cached value of the sequence.       |
     +------------+----------------------------------------------+
     | min        | The minimum permitted value of the sequence. |
     +------------+----------------------------------------------+
     | max        | The maximum permitted value of the sequence. |
     +------------+----------------------------------------------+
     | cache_size | The number of values that will be cached in  |
     |            | this handle.                                 |
     +------------+----------------------------------------------+               
     | flags      | The flags value for the sequence.            |               
     +------------+----------------------------------------------+

   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/seq_stat.html>`__

.. function:: stat_print(flags=0)

   Prints diagnostic information.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/seq_stat.html>`__

.. function:: get_range()

   Returns a tuple representing the range of values in the sequence.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/seq_set_range.html>`__

.. function:: set_range((min,max))

   Configure a sequence range.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/seq_set_range.html>`__

