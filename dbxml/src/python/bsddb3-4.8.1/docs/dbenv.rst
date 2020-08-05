.. $Id: dbenv.rst 720 2009-07-23 14:07:25Z jcea $

=====
DBEnv
=====

DBEnv Attributes
----------------

.. function:: DBEnv(flags=0)

   database home directory (read-only)

DBEnv Methods
-------------

.. function:: DBEnv(flags=0)

   Constructor.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_class.html>`__

.. function:: set_rpc_server(host, cl_timeout=0, sv_timeout=0)

   Establishes a connection for this dbenv to a RPC server.
   This function is not available if linked to Berkeley DB 4.8 or up.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_rpc_server.html>`__

.. function:: close(flags=0)

   Close the database environment, freeing resources.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_close.html>`__

.. function:: open(homedir, flags=0, mode=0660)

   Prepare the database environment for use.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_open.html>`__

.. function:: remove(homedir, flags=0)

   Remove a database environment.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_remove.html>`__

.. function:: dbremove(file, database=None, txn=None, flags=0)

   Removes the database specified by the file and database parameters.
   If no database is specified, the underlying file represented by file
   is removed, incidentally removing all of the databases it contained.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_dbremove.html>`__

.. function:: dbrename(file, database=None, newname, txn=None, flags=0)

   Renames the database specified by the file and database parameters to
   newname. If no database is specified, the underlying file represented
   by file is renamed, incidentally renaming all of the databases it
   contained.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_dbrename.html>`__

.. function:: fileid_reset(file, flags=0)

   All databases contain an ID string used to identify the database in
   the database environment cache. If a physical database file is
   copied, and used in the same environment as another file with the
   same ID strings, corruption can occur. The DB_ENV->fileid_reset
   method creates new ID strings for all of the databases in the
   physical file.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_fileid_reset.html>`__

.. function:: set_encrypt(passwd, flags=0)

   Set the password used by the Berkeley DB library to perform
   encryption and decryption.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_encrypt.html>`__

.. function:: get_encrypt_flags()

   Returns the encryption flags.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_encrypt.html>`__

.. function:: get_timeout(flags)

   Returns a timeout value, in microseconds.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_timeout.html>`__

.. function:: set_timeout(timeout, flags)

   Sets timeout values for locks or transactions in the database
   environment.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_timeout.html>`__

.. function:: set_shm_key(key)

   Specify a base segment ID for Berkeley DB environment shared memory
   regions created in system memory on VxWorks or systems supporting
   X/Open-style shared memory interfaces; for example, UNIX systems
   supporting shmget(2) and related System V IPC interfaces.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_shm_key.html>`__

.. function:: get_shm_key()

   Returns the base segment ID.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_shm_key.html>`__

.. function:: set_cachesize(gbytes, bytes, ncache=0)

   Set the size of the shared memory buffer pool.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_cachesize.html>`__

.. function:: get_cachesize()

   Returns a tuple with the current size and composition of the cache.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_cachesize.html>`__

.. function:: set_data_dir(dir)

   Set the environment data directory. You can call this function
   multiple times, adding new directories.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_data_dir.html>`__

.. function:: get_data_dirs()

   Return a tuple with the directories.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_data_dir.html>`__

.. function:: set_flags(flags, onoff)

   Set additional flags for the DBEnv. The onoff parameter specifes if
   the flag is set or cleared.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_flags.html>`__

.. function:: set_tmp_dir(dir)

   Set the directory to be used for temporary files.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_tmp_dir.html>`__

.. function:: set_get_returns_none(flag)

   By default when DB.get or DBCursor.get, get_both, first, last, next
   or prev encounter a DB_NOTFOUND error they return None instead of
   raising DBNotFoundError. This behaviour emulates Python dictionaries
   and is convenient for looping.

   You can use this method to toggle that behaviour for all of the
   aformentioned methods or extend it to also apply to the DBCursor.set,
   set_both, set_range, and set_recno methods. Supported values of
   flag:

   - **0** all DB and DBCursor get and set methods will raise a
     DBNotFoundError rather than returning None.

   - **1** *Default in module version <4.2.4*  The DB.get and
     DBCursor.get, get_both, first, last, next and prev methods return
     None.

   - **2** *Default in module version >=4.2.4* Extends the behaviour of
     **1** to the DBCursor set, set_both, set_range and set_recno
     methods.

   The default of returning None makes it easy to do things like this
   without having to catch DBNotFoundError (KeyError)::

                    data = mydb.get(key)
                    if data:
                        doSomething(data)

   or this::

                    rec = cursor.first()
                    while rec:
                        print rec
                        rec = cursor.next()

   Making the cursor set methods return None is useful in order to do
   this::

                    rec = mydb.set()
                    while rec:
                        key, val = rec
                        doSomething(key, val)
                        rec = mydb.next()

   The downside to this it that it is inconsistent with the rest of the
   package and noticeably diverges from the Oracle Berkeley DB API. If
   you prefer to have the get and set methods raise an exception when a
   key is not found, use this method to tell them to do so.

   Calling this method on a DBEnv object will set the default for all
   DB's later created within that environment. Calling it on a DB
   object sets the behaviour for that DB only.

   The previous setting is returned.

.. function:: set_private(object)

   Link an arbitrary object to the DBEnv.

.. function:: get_private()

   Give the object linked to the DBEnv.
   
.. function:: set_lg_bsize(size)

   Set the size of the in-memory log buffer, in bytes.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_lg_bsize.html>`__

.. function:: set_lg_dir(dir)

   The path of a directory to be used as the location of logging files.
   Log files created by the Log Manager subsystem will be created in
   this directory.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_lg_dir.html>`__

.. function:: set_lg_max(size)

   Set the maximum size of a single file in the log, in bytes.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_lg_max.html>`__

.. function:: get_lg_max(size)

   Returns the maximum log file size.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_lg_max.html>`__

.. function:: set_lg_regionmax(size)

   Set the maximum size of a single region in the log, in bytes.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_lg_regionmax.html>`__

.. function:: set_lk_detect(mode)

   Set the automatic deadlock detection mode.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_lk_detect.html>`__

.. function:: set_lk_max(max)

   Set the maximum number of locks. (This method is deprecated.)
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_lk_max.html>`__

.. function:: set_lk_max_locks(max)

   Set the maximum number of locks supported by the Berkeley DB lock
   subsystem.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_lk_max_locks.html>`__

.. function:: set_lk_max_lockers(max)

   Set the maximum number of simultaneous locking entities supported by
   the Berkeley DB lock subsystem.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_lk_max_lockers.html>`__

.. function:: set_lk_max_objects(max)

   Set the maximum number of simultaneously locked objects supported by
   the Berkeley DB lock subsystem.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_lk_max_lockers.html>`__

.. function:: set_mp_mmapsize(size)

   Files that are opened read-only in the memory pool (and that satisfy
   a few other criteria) are, by default, mapped into the process
   address space instead of being copied into the local cache. This can
   result in better-than-usual performance, as available virtual memory
   is normally much larger than the local cache, and page faults are
   faster than page copying on many systems. However, in the presence
   of limited virtual memory it can cause resource starvation, and in
   the presence of large databases, it can result in immense process
   sizes.

   This method sets the maximum file size, in bytes, for a file to be
   mapped into the process address space. If no value is specified, it
   defaults to 10MB.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_mp_mmapsize.html>`__

.. function:: log_archive(flags=0)

   Returns a list of log or database file names. By default,
   log_archive returns the names of all of the log files that are no
   longer in use (e.g., no longer involved in active transactions), and
   that may safely be archived for catastrophic recovery and then
   removed from the system.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/log_archive.html>`__

.. function:: log_flush()

   Force log records to disk. Useful if the environment, database or
   transactions are used as ACI, instead of ACID. For example, if the
   environment is opened as DB_TXN_NOSYNC.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/log_flush.html>`__

.. function:: log_set_config(flags, onoff)

   Configures the Berkeley DB logging subsystem.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_log_set_config.html>`__

.. function:: lock_detect(atype, flags=0)

   Run one iteration of the deadlock detector, returns the number of
   transactions aborted.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/lock_detect.html>`__

.. function:: lock_get(locker, obj, lock_mode, flags=0)

   Acquires a lock and returns a handle to it as a DBLock object. The
   locker parameter is an integer representing the entity doing the
   locking, and obj is an object representing the item to be locked.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/lock_get.html>`__

.. function:: lock_id()

   Acquires a locker id, guaranteed to be unique across all threads and
   processes that have the DBEnv open.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/lock_id.html>`__

.. function:: lock_id_free(id)

   Frees a locker ID allocated by the "dbenv.lock_id()" method.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/lock_id_free.html>`__

.. function:: lock_put(lock)

   Release the lock.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/lock_put.html>`__

.. function:: lock_stat(flags=0)

   Returns a dictionary of locking subsystem statistics with the
   following keys:

    +----------------+---------------------------------------------+
    | id             | Last allocated lock ID.                     |
    +----------------+---------------------------------------------+
    | cur_maxid      | The current maximum unused locker ID.       |
    +----------------+---------------------------------------------+
    | nmodes         | Number of lock modes.                       |
    +----------------+---------------------------------------------+
    | maxlocks       | Maximum number of locks possible.           |
    +----------------+---------------------------------------------+
    | maxlockers     | Maximum number of lockers possible.         |
    +----------------+---------------------------------------------+
    | maxobjects     | Maximum number of objects possible.         |
    +----------------+---------------------------------------------+
    | nlocks         | Number of current locks.                    |
    +----------------+---------------------------------------------+
    | maxnlocks      | Maximum number of locks at once.            |
    +----------------+---------------------------------------------+
    | nlockers       | Number of current lockers.                  |
    +----------------+---------------------------------------------+
    | nobjects       | Number of current lock objects.             |
    +----------------+---------------------------------------------+
    | maxnobjects    | Maximum number of lock objects at once.     |
    +----------------+---------------------------------------------+
    | maxnlockers    | Maximum number of lockers at once.          |
    +----------------+---------------------------------------------+
    | nrequests      | Total number of locks requested.            |
    +----------------+---------------------------------------------+
    | nreleases      | Total number of locks released.             |
    +----------------+---------------------------------------------+
    | nupgrade       | Total number of locks upgraded.             |
    +----------------+---------------------------------------------+
    | ndowngrade     | Total number of locks downgraded.           |
    +----------------+---------------------------------------------+
    | lock_wait      | The number of lock requests not immediately |
    |                | available due to conflicts, for which the   |
    |                | thread of control waited.                   |
    +----------------+---------------------------------------------+
    | lock_nowait    | The number of lock requests not immediately | 
    |                | available due to conflicts, for which the   |
    |                | thread of control did not wait.             |
    +----------------+---------------------------------------------+
    | ndeadlocks     | Number of deadlocks.                        |
    +----------------+---------------------------------------------+
    | locktimeout    | Lock timeout value.                         |
    +----------------+---------------------------------------------+
    | nlocktimeouts  | The number of lock requests that have timed |
    |                | out.                                        |
    +----------------+---------------------------------------------+
    | txntimeout     | Transaction timeout value.                  |
    +----------------+---------------------------------------------+
    | ntxntimeouts   | The number of transactions that have timed  |
    |                | out. This value is also a component of      |
    |                | ndeadlocks, the total number of deadlocks   |
    |                | detected.                                   |
    +----------------+---------------------------------------------+
    | objs_wait      | The number of requests to allocate or       |
    |                | deallocate an object for which the thread   |
    |                | of control waited.                          |
    +----------------+---------------------------------------------+
    | objs_nowait    | The number of requests to allocate or       |
    |                | deallocate an object for which the thread   |
    |                | of control did not wait.                    |
    +----------------+---------------------------------------------+
    | lockers_wait   | The number of requests to allocate or       |
    |                | deallocate a locker for which the thread of |
    |                | control waited.                             |
    +----------------+---------------------------------------------+
    | lockers_nowait | The number of requests to allocate or       |
    |                | deallocate a locker for which the thread of |
    |                | control did not wait.                       |
    +----------------+---------------------------------------------+
    | locks_wait     | The number of requests to allocate or       |
    |                | deallocate a lock structure for which the   |
    |                | thread of control waited.                   |
    +----------------+---------------------------------------------+
    | locks_nowait   | The number of requests to allocate or       |
    |                | deallocate a lock structure for which the   |
    |                | thread of control did not wait.             |
    +----------------+---------------------------------------------+
    | hash_len       | Maximum length of a lock hash bucket.       |
    +----------------+---------------------------------------------+
    | regsize        | Size of the region.                         |
    +----------------+---------------------------------------------+
    | region_wait    | Number of times a thread of control was     |
    |                | forced to wait before obtaining the region  |
    |                | lock.                                       |
    +----------------+---------------------------------------------+
    | region_nowait  | Number of times a thread of control was     |
    |                | able to obtain the region lock  without     |
    |                | waiting.                                    |
    +----------------+---------------------------------------------+

   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/lock_stat.html>`__

.. function:: get_tx_max()

   Returns the number of active transactions.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_tx_max.html>`__

.. function:: set_tx_max(max)

   Set the maximum number of active transactions.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_tx_max.html>`__

.. function:: get_tx_timestamp()

   Returns the recovery timestamp.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_tx_timestamp.html>`__

.. function:: set_tx_timestamp(timestamp)

   Recover to the time specified by timestamp rather than to the most
   current possible date.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_tx_timestamp.html>`__

.. function:: txn_begin(parent=None, flags=0)

   Creates and begins a new transaction. A DBTxn object is returned.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/txn_begin.html>`__

.. function:: txn_checkpoint(kbyte=0, min=0, flag=0)

   Flushes the underlying memory pool, writes a checkpoint record to the
   log and then flushes the log.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/txn_checkpoint.html>`__

.. function:: txn_stat(flags=0)

   Return a dictionary of transaction statistics with the following
   keys:

    +--------------+---------------------------------------------+
    | last_ckp     | The LSN of the last checkpoint.             |
    +--------------+---------------------------------------------+
    | time_ckp     | Time the last completed checkpoint finished |
    |              | (as the number of seconds since the Epoch,  |
    |              | returned by the IEEE/ANSI Std 1003.1 POSIX  |
    |              | time interface).                            |
    +--------------+---------------------------------------------+
    | last_txnid   | Last transaction ID allocated.              |
    +--------------+---------------------------------------------+
    | maxtxns      | Max number of active transactions possible. |
    +--------------+---------------------------------------------+
    | nactive      | Number of transactions currently active.    |
    +--------------+---------------------------------------------+
    | maxnactive   | Max number of active transactions at once.  |
    +--------------+---------------------------------------------+
    | nsnapshot    | The number of transactions on the snapshot  |
    |              | list. These are transactions which modified |
    |              | a database opened with DB_MULTIVERSION, and |
    |              | which have committed or aborted, but the    |
    |              | copies of pages they created are still in   |
    |              | the cache.                                  |
    +--------------+---------------------------------------------+
    | maxnsnapshot | The maximum number of transactions on the   |
    |              | snapshot list at any one time.              |
    +--------------+---------------------------------------------+
    | nbegins      | Number of transactions that have begun.     |
    +--------------+---------------------------------------------+
    | naborts      | Number of transactions that have aborted.   |
    +--------------+---------------------------------------------+
    | ncommits     | Number of transactions that have committed. |
    +--------------+---------------------------------------------+
    | nrestores    | Number of transactions that have been       |
    |              | restored.                                   |
    +--------------+---------------------------------------------+
    | regsize      | Size of the region.                         |
    +--------------+---------------------------------------------+
    | region_wait  | Number of times that a thread of control    |
    |              | was forced to wait before obtaining the     |
    |              | region lock.                                |
    +--------------+---------------------------------------------+
    | region_nowait| Number of times that a thread of control    |
    |              | was able to obtain the region lock without  |
    |              | waiting.                                    |
    +--------------+---------------------------------------------+

   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/txn_stat.html>`__

.. function:: txn_stat_print(flags=0)

   Displays the transaction subsystem statistical information.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/txn_stat.html>`__

.. function:: lsn_reset(file=None,flags=0)

   This method allows database files to be moved from one transactional
   database environment to another.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_lsn_reset.html>`__

.. function:: log_stat(flags=0)

   Returns a dictionary of logging subsystem statistics with the
   following keys:

    +-------------------+---------------------------------------------+
    | magic             | The magic number that identifies a file as  |
    |                   | a log file.                                 |
    +-------------------+---------------------------------------------+
    | version           | The version of the log file type.           |
    +-------------------+---------------------------------------------+
    | mode              | The mode of any created log files.          |
    +-------------------+---------------------------------------------+
    | lg_bsize          | The in-memory log record cache size.        |
    +-------------------+---------------------------------------------+
    | lg_size           | The log file size.                          |
    +-------------------+---------------------------------------------+
    | record            | The number of records written to this log.  |
    +-------------------+---------------------------------------------+
    | w_mbytes          | The number of megabytes written to this     |
    |                   | log.                                        |
    +-------------------+---------------------------------------------+
    | w_bytes           | The number of bytes over and above w_mbytes |
    |                   | written to this log.                        |
    +-------------------+---------------------------------------------+
    | wc_mbytes         | The number of megabytes written to this log |
    |                   | since the last checkpoint.                  |
    +-------------------+---------------------------------------------+
    | wc_bytes          | The number of bytes over and above          |
    |                   | wc_mbytes written to this log since the     |
    |                   | last checkpoint.                            |
    +-------------------+---------------------------------------------+
    | wcount            | The number of times the log has been        |
    |                   | written to disk.                            |
    +-------------------+---------------------------------------------+
    | wcount_fill       | The number of times the log has been        |
    |                   | written to disk because the in-memory log   |
    |                   | record cache filled up.                     |
    +-------------------+---------------------------------------------+
    | rcount            | The number of times the log has been read   |
    |                   | from disk.                                  |
    +-------------------+---------------------------------------------+
    | scount            | The number of times the log has been        |
    |                   | flushed to disk.                            |
    +-------------------+---------------------------------------------+
    | cur_file          | The current log file number.                |
    +-------------------+---------------------------------------------+
    | cur_offset        | The byte offset in the current log file.    |
    +-------------------+---------------------------------------------+
    | disk_file         | The log file number of the last record      |
    |                   | known to be on disk.                        |
    +-------------------+---------------------------------------------+
    | disk_offset       | The byte offset of the last record known to |
    |                   | be on disk.                                 |
    +-------------------+---------------------------------------------+
    | maxcommitperflush | The maximum number of commits contained in  |
    |                   | a single log flush.                         |
    +-------------------+---------------------------------------------+
    | mincommitperflush | The minimum number of commits contained in  |
    |                   | a single log flush that contained a commit. |
    +-------------------+---------------------------------------------+
    | regsize           | The size of the log region, in bytes.       |
    +-------------------+---------------------------------------------+
    | region_wait       | The number of times that a thread of        |
    |                   | control was forced to wait before obtaining |
    |                   | the log region mutex.                       |
    +-------------------+---------------------------------------------+
    | region_nowait     | The number of times that a thread of        |
    |                   | control was able to obtain the log region   |
    |                   | mutex without waiting.                      |
    +-------------------+---------------------------------------------+

   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/log_stat.html>`__

.. function:: txn_recover()

   Returns a list of tuples (GID, TXN) of transactions prepared but
   still unresolved. This is used while doing environment recovery in an
   application using distributed transactions.

   This method must be called only from a single thread at a time. It
   should be called after DBEnv recovery.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/txn_recover.html>`__

.. function:: set_verbose(which, onoff)

   Turns specific additional informational and debugging messages in the
   Berkeley DB message output on and off. To see the additional
   messages, verbose messages must also be configured for the
   application.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_verbose.html>`__

.. function:: get_verbose(which)

   Returns whether the specified *which* parameter is currently set or
   not.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_set_verbose.html>`__

.. function:: set_event_notify(eventFunc)

   Configures a callback function which is called to notify the process
   of specific Berkeley DB events.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/env_event_notify.html>`__

.. function:: mutex_set_max(value)

   Configure the total number of mutexes to allocate.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/mutex_set_max.html>`__

.. function:: mutex_get_max()

   Returns the total number of mutexes allocated.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/mutex_set_max.html>`__

.. function:: mutex_set_increment(value)

   Configure the number of additional mutexes to allocate.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/mutex_set_max.html>`__

.. function:: mutex_get_increment()

   Returns the number of additional mutexes to allocate.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/mutex_set_max.html>`__

.. function:: mutex_set_align(align)

   Set the mutex alignment, in bytes.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/mutex_set_align.html>`__

.. function:: mutex_get_align()

   Returns the mutex alignment, in bytes.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/mutex_set_align.html>`__

.. function:: mutex_set_tas_spins(tas_spins)

   Specify that test-and-set mutexes should spin tas_spins times without
   blocking. Check the default values in the Oracle webpage.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/mutex_set_tas_spins.html>`__

.. function:: mutex_get_tas_spins()

   Returns the test-and-set spin count.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/mutex_set_tas_spins.html>`__

DBEnv Replication Manager Methods
---------------------------------

This module automates many of the tasks needed to provide replication
abilities in a Berkeley DB system. The module is fairly limited, but
enough in many cases. Users more demanding must use the **full** Base
Replication API.

This module requires POSIX support, so you must compile Berkeley DB with
it if you want to be able to use the Replication Manager.

.. function:: repmgr_start(nthreads, flags)

   Starts the replication manager.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/repmgr_start.html>`__

.. function:: repmgr_set_local_site(host, port, flags=0)

   Specifies the host identification string and port number for the
   local system.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/repmgr_local_site.html>`__

.. function:: repmgr_add_remote_site(host, port, flags=0)

   Adds a new replication site to the replication manager's list of
   known sites. It is not necessary for all sites in a replication group
   to know about all other sites in the group.

   Method returns the environment ID assigned to the remote site.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/repmgr_remote_site.html>`__

.. function:: repmgr_set_ack_policy(ack_policy)

   Specifies how master and client sites will handle acknowledgment of
   replication messages which are necessary for "permanent" records.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/repmgr_ack_policy.html>`__

.. function:: repmgr_get_ack_policy()

   Returns the replication manager's client acknowledgment policy.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/repmgr_ack_policy.html>`__

.. function:: repmgr_site_list()

   Returns a dictionary with the status of the sites currently known by
   the replication manager.
   
   The keys are the Environment ID assigned by the replication manager.
   This is the same value that is passed to the application's event
   notification function for the DB_EVENT_REP_NEWMASTER event. 

   The values are tuples containing the hostname, the TCP/IP port number
   and the link status.

   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/repmgr_site_list.html>`__

.. function:: repmgr_stat(flags=0)

   Returns a dictionary with the replication manager statistics. Keys
   are:

   +-----------------+-------------------------------------------------+
   | perm_failed     | The number of times a message critical for      |
   |                 | maintaining database integrity (for example, a  |
   |                 | transaction commit), originating at this site,  |
   |                 | did not receive sufficient acknowledgement from |
   |                 | clients, according to the configured            |
   |                 | acknowledgement policy and acknowledgement      |
   |                 | timeout.                                        |
   +-----------------+-------------------------------------------------+
   | msgs_queued     | The number of outgoing messages which could not |
   |                 | be transmitted immediately, due to a full       |
   |                 | network buffer, and had to be queued for later  |
   |                 | delivery.                                       |
   +-----------------+-------------------------------------------------+
   | msgs_dropped    | The number of outgoing messages that were       |
   |                 | completely dropped, because the outgoing        |
   |                 | message queue was full. (Berkeley DB            |
   |                 | replication is tolerant of dropped messages,    |
   |                 | and will automatically request retransmission   |
   |                 | of any missing messages as needed.)             |
   +-----------------+-------------------------------------------------+
   | connection_drop | The number of times an existing TCP/IP          |
   |                 | connection failed.                              |
   +-----------------+-------------------------------------------------+
   | connect_fail    | The number of times an attempt to open a new    |
   |                 | TCP/IP connection failed.                       |
   +-----------------+-------------------------------------------------+

   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/repmgr_stat.html>`__

.. function:: repmgr_stat_print(flags=0)

   Displays the replication manager statistical information.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/repmgr_stat.html>`__


DBEnv Replication Methods
-------------------------

.. function:: rep_elect(nsites, nvotes)

   Holds an election for the master of a replication group.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_elect.html>`__

.. function:: rep_set_transport(envid, transportFunc)

   Initializes the communication infrastructure for a database
   environment participating in a replicated application.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_transport.html>`__

.. function:: rep_process_messsage(control, rec, envid)

   Processes an incoming replication message sent by a member of the
   replication group to the local database environment.

   Returns a two element tuple.

   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_message.html>`__

.. function:: rep_start(flags, cdata=None)

   Configures the database environment as a client or master in a group
   of replicated database environments.

   The DB_ENV->rep_start method is not called by most replication
   applications. It should only be called by applications implementing
   their own network transport layer, explicitly holding replication
   group elections and handling replication messages outside of the
   replication manager framework.

   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_start.html>`__

.. function:: rep_sync()

   Forces master synchronization to begin for this client. This method
   is the other half of setting the DB_REP_CONF_DELAYCLIENT flag via the
   DB_ENV->rep_set_config method.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_sync.html>`__

.. function:: rep_set_config(which, onoff)

   Configures the Berkeley DB replication subsystem.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_config.html>`__

.. function:: rep_get_config(which)

   Returns whether the specified which parameter is currently set or
   not.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_config.html>`__

.. function:: rep_set_limit(bytes)

   Sets a byte-count limit on the amount of data that will be
   transmitted from a site in response to a single message processed by
   the DB_ENV->rep_process_message method. The limit is not a hard
   limit, and the record that exceeds the limit is the last record to be
   sent.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_limit.html>`__

.. function:: rep_get_limit()

   Gets a byte-count limit on the amount of data that will be
   transmitted from a site in response to a single message processed by
   the DB_ENV->rep_process_message method. The limit is not a hard
   limit, and the record that exceeds the limit is the last record to be
   sent.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_limit.html>`__

.. function:: rep_set_request(minimum, maximum)

   Sets a threshold for the minimum and maximum time that a client
   waits before requesting retransmission of a missing message.
   Specifically, if the client detects a gap in the sequence of incoming
   log records or database pages, Berkeley DB will wait for at least min
   microseconds before requesting retransmission of the missing record.
   Berkeley DB will double that amount before requesting the same
   missing record again, and so on, up to a maximum threshold of max
   microseconds.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_request.html>`__

.. function:: rep_get_request()

   Returns a tuple with the minimum and maximum number of microseconds a
   client waits before requesting retransmission.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_request.html>`__

.. function:: rep_set_nsites(nsites)

   Specifies the total number of sites in a replication group.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_nsites.html>`__

.. function:: rep_get_nsites()

   Returns the total number of sites in the replication group.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_nsites.html>`__

.. function:: rep_set_priority(priority)

   Specifies the database environment's priority in replication group
   elections. The priority must be a positive integer, or 0 if this
   environment cannot be a replication group master.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_priority.html>`__

.. function:: rep_get_priority()

   Returns the database environment priority.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_priority.html>`__

.. function:: rep_set_timeout(which, timeout)

   Specifies a variety of replication timeout values.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_timeout.html>`__

.. function:: rep_get_timeout(which)

   Returns the timeout value for the specified *which* parameter.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_timeout.html>`__

.. function:: rep_set_clockskew(fast, slow)

   Sets the clock skew ratio among replication group members based on
   the fastest and slowest measurements among the group for use with
   master leases.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_clockskew.html>`__

.. function:: rep_get_clockskew()

   Returns a tuple with the current clock skew values.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_clockskew.html>`__
   
.. function:: rep_stat(flags=0)

   Returns a dictionary with the replication subsystem statistics. Keys
   are:

   +---------------------+---------------------------------------------+
   | st_bulk_fills       | The number of times the bulk buffer filled  |
   |                     | up, forcing the buffer content to be sent.  |
   +---------------------+---------------------------------------------+
   | bulk_overflows      | The number of times a record was bigger     |
   |                     | than the entire bulk buffer, and therefore  |
   |                     | had to be sent as a singleton.              |
   +---------------------+---------------------------------------------+
   | bulk_records        | The number of records added to a bulk       |
   |                     | buffer.                                     |
   +---------------------+---------------------------------------------+
   | bulk_transfers      | The number of bulk buffers transferred (via |
   |                     | a call to the application's send function). |
   +---------------------+---------------------------------------------+
   | client_rerequests   | The number of times this client site        |
   |                     | received a "re-request" message, indicating |
   |                     | that a request it previously sent to        |
   |                     | another client could not be serviced by     |
   |                     | that client. (Compare to client_svc_miss.)  |
   +---------------------+---------------------------------------------+
   | client_svc_miss     | The number of "request" type messages       |
   |                     | received by this client that could not be   |
   |                     | processed, forcing the originating          |
   |                     | requester to try sending the request to the |
   |                     | master (or another client).                 |
   +---------------------+---------------------------------------------+
   | client_svc_req      | The number of "request" type messages       |
   |                     | received by this client. ("Request"         |
   |                     | messages are usually sent from a client to  |
   |                     | the master, but a message marked with the   |
   |                     | DB_REP_ANYWHERE flag in the invocation of   |
   |                     | the application's send function may be sent |
   |                     | to another client instead.)                 |
   +---------------------+---------------------------------------------+
   | dupmasters          | The number of duplicate master conditions   |
   |                     | originally detected at this site.           |
   +---------------------+---------------------------------------------+
   | egen                | The current election generation number.     |
   +---------------------+---------------------------------------------+
   | election_cur_winner | The election winner.                        |
   +---------------------+---------------------------------------------+
   | election_gen        | The election generation number.             |
   +---------------------+---------------------------------------------+
   | election_lsn        | The maximum LSN of election winner.         |
   +---------------------+---------------------------------------------+
   | election_nsites     | The number sites responding to this site    |
   |                     | during the last election.                   |
   +---------------------+---------------------------------------------+
   | election_nvotes     | The number of votes required in the last    |
   |                     | election.                                   |
   +---------------------+---------------------------------------------+
   | election_priority   | The election priority.                      |
   +---------------------+---------------------------------------------+
   | election_sec        | The number of seconds the last election     |
   |                     | took (the total election time is            |
   |                     | election_sec plus election_usec).           |
   +---------------------+---------------------------------------------+
   | election_status     | The current election phase (0 if no         |
   |                     | election is in progress).                   |
   +---------------------+---------------------------------------------+
   | election_tiebreaker | The election tiebreaker value.              |
   +---------------------+---------------------------------------------+
   | election_usec       | The number of microseconds the last         |
   |                     | election took (the total election time is   |
   |                     | election_sec plus election_usec).           |
   +---------------------+---------------------------------------------+
   | election_votes      | The number of votes received in the last    |
   |                     | election.                                   |
   +---------------------+---------------------------------------------+
   | elections           | The number of elections held.               |
   +---------------------+---------------------------------------------+
   | elections_won       | The number of elections won.                |
   +---------------------+---------------------------------------------+
   | env_id              | The current environment ID.                 |
   +---------------------+---------------------------------------------+
   | env_priority        | The current environment priority.           |
   +---------------------+---------------------------------------------+
   | gen                 | The current generation number.              |
   +---------------------+---------------------------------------------+
   | log_duplicated      | The number of duplicate log records         |
   |                     | received.                                   |
   +---------------------+---------------------------------------------+
   | log_queued          | The number of log records currently queued. |
   +---------------------+---------------------------------------------+
   | log_queued_max      | The maximum number of log records ever      |
   |                     | queued at once.                             |
   +---------------------+---------------------------------------------+
   | log_queued_total    | The total number of log records queued.     |
   +---------------------+---------------------------------------------+
   | log_records         | The number of log records received and      |
   |                     | appended to the log.                        |
   +---------------------+---------------------------------------------+
   | log_requested       | The number of times log records were missed |
   |                     | and requested.                              |
   +---------------------+---------------------------------------------+
   | master              | The current master environment ID.          |
   +---------------------+---------------------------------------------+
   | master_changes      | The number of times the master has changed. |
   +---------------------+---------------------------------------------+
   | max_lease_sec       | The number of seconds of the longest lease  |
   |                     | (the total lease time is max_lease_sec plus |
   |                     | max_lease_usec).                            |
   +---------------------+---------------------------------------------+
   | max_lease_usec      | The number of microseconds of the longest   |
   |                     | lease (the total lease time is              |
   |                     | max_lease_sec plus max_lease_usec).         |
   +---------------------+---------------------------------------------+
   | max_perm_lsn        | The LSN of the maximum permanent log        |
   |                     | record, or 0 if there are no permanent log  |
   |                     | records.                                    |
   +---------------------+---------------------------------------------+
   | msgs_badgen         | The number of messages received with a bad  |
   |                     | generation number.                          |
   +---------------------+---------------------------------------------+
   | msgs_processed      | The number of messages received and         |
   |                     | processed.                                  |
   +---------------------+---------------------------------------------+
   | msgs_recover        | The number of messages ignored due to       |
   |                     | pending recovery.                           |
   +---------------------+---------------------------------------------+
   | msgs_send_failures  | The number of failed message sends.         |
   +---------------------+---------------------------------------------+
   | msgs_sent           | The number of messages sent.                |
   +---------------------+---------------------------------------------+
   | newsites            | The number of new site messages received.   |
   +---------------------+---------------------------------------------+
   | next_lsn            | In replication environments configured as   |
   |                     | masters, the next LSN expected. In          |
   |                     | replication environments configured as      |
   |                     | clients, the next LSN to be used.           |
   +---------------------+---------------------------------------------+
   | next_pg             | The next page number we expect to receive.  |
   +---------------------+---------------------------------------------+
   | nsites              | The number of sites used in the last        |
   |                     | election.                                   |
   +---------------------+---------------------------------------------+
   | nthrottles          | Transmission limited. This indicates the    |
   |                     | number of times that data transmission was  |
   |                     | stopped to limit the amount of data sent in |
   |                     | response to a single call to                |
   |                     | DB_ENV->rep_process_message.                |
   +---------------------+---------------------------------------------+
   | outdated            | The number of outdated conditions detected. |
   +---------------------+---------------------------------------------+
   | pg_duplicated       | The number of duplicate pages received.     |
   +---------------------+---------------------------------------------+
   | pg_records          | The number of pages received and stored.    |
   +---------------------+---------------------------------------------+
   | pg_requested        | The number of pages missed and requested    |
   |                     | from the master.                            |
   +---------------------+---------------------------------------------+
   | startsync_delayed   | The number of times the client had to delay |
   |                     | the start of a cache flush operation        |
   |                     | (initiated by the master for an impending   |
   |                     | checkpoint) because it was missing some     |
   |                     | previous log record(s).                     |
   +---------------------+---------------------------------------------+
   | startup_complete    | The client site has completed its startup   |
   |                     | procedures and is now handling live records |
   |                     | from the master.                            |
   +---------------------+---------------------------------------------+
   | status              |The current replication mode. Set to         |
   |                     | DB_REP_MASTER if the environment is a       |
   |                     | replication master, DB_REP_CLIENT if the    |
   |                     | environment is a replication client, or 0   |
   |                     | if replication is not configured.           |
   +---------------------+---------------------------------------------+
   | txns_applied        | The number of transactions applied.         |
   +---------------------+---------------------------------------------+
   | waiting_lsn         | The LSN of the first log record we have     |
   |                     | after missing log records being waited for, |
   |                     | or 0 if no log records are currently        |
   |                     | missing.                                    |
   +---------------------+---------------------------------------------+
   | waiting_pg          | The page number of the first page we have   |
   |                     | after missing pages being waited for, or 0  |
   |                     | if no pages are currently missing.          |
   +---------------------+---------------------------------------------+

   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_stat.html>`__

.. function:: rep_stat_print(flags=0)

   Displays the replication subsystem statistical information.
   `More info...
   <http://www.oracle.com/technology/documentation/berkeley-db/db/
   api_c/rep_stat.html>`__

