.. $Id: dbtxn.rst 699 2009-06-10 11:38:56Z jcea $

=====
DBTxn
=====

DBTxn Methods
-------------

.. function:: abort()

   Aborts the transaction
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/txn_abort.html>`__

.. function:: commit(flags=0)

   Ends the transaction, committing any changes to the databases.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/txn_commit.html>`__

.. function:: id()

   The txn_id function returns the unique transaction id associated with
   the specified transaction.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/txn_id.html>`__

.. function:: prepare(gid)

   Initiates the beginning of a two-phase commit. A global identifier
   parameter is required, which is a value unique across all processes
   involved in the commit. It must be a string of DB_GID_SIZE bytes.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/txn_prepare.html>`__

.. function:: discard()

   This method frees up all the per-process resources associated with
   the specified transaction, neither committing nor aborting the
   transaction. The transaction will be keep in "unresolved" state. This
   call may be used only after calls to "dbenv.txn_recover()". A
   "unresolved" transaction will be returned again thru new calls to
   "dbenv.txn_recover()".
   
   For example, when there are multiple global transaction managers
   recovering transactions in a single Berkeley DB environment, any
   transactions returned by "dbenv.txn_recover()" that are not handled
   by the current global transaction manager should be discarded using
   "txn.discard()".

   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/txn_discard.html>`__

.. function:: set_timeout(timeout, flags)

   Sets timeout values for locks or transactions for the specified
   transaction.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/txn_set_timeout.html>`__

.. function:: get_name(name)

   Returns the string associated with the transaction.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/txn_set_name.html>`__

.. function:: set_name(name)

   Associates the specified string with the transaction.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/txn_set_name.html>`__

