class DbTxn
{
public:
	int abort();
	int commit(u_int32_t flags);
	int discard(u_int32_t flags);
	u_int32_t id();
	int set_timeout(db_timeout_t timeout, u_int32_t flags);

private:
	// Note: use DbEnv::txn_begin() to get pointers to a DbTxn,
	// and call DbTxn::abort() or DbTxn::commit rather than
	// delete to release them.
	//
	DbTxn();
	virtual ~DbTxn();
};

class DbEnv
{
public:
	// After using this constructor, you can set any needed
	// parameters for the environment using the set_* methods.
	// Then call open() to finish initializing the environment
	// and attaching it to underlying files.
	//
	DbEnv(u_int32_t flags);
	virtual ~DbEnv();

	// These methods match those in the C interface.
	//
	virtual int close(u_int32_t);
	virtual int dbremove(DbTxn *txn, const char *name, const char *subdb,
	    u_int32_t flags);
	virtual int dbrename(DbTxn *txn, const char *name, const char *subdb,
	    const char *newname, u_int32_t flags);
	virtual int open(const char *, u_int32_t, int);
	virtual int remove(const char *, u_int32_t);
	virtual int set_cachesize(u_int32_t, u_int32_t, int);
	virtual int set_data_dir(const char *);
	virtual int set_encrypt(const char *, u_int32_t);
	virtual void set_errpfx(const char *);
	virtual int set_flags(u_int32_t, int);
	virtual int set_lg_bsize(u_int32_t);
	virtual int set_lg_dir(const char *);
	virtual int set_lg_max(u_int32_t);
	virtual int set_lg_regionmax(u_int32_t);
	virtual int set_lk_detect(u_int32_t);
	virtual int set_lk_max_lockers(u_int32_t);
	virtual int set_lk_max_locks(u_int32_t);
	virtual int set_lk_max_objects(u_int32_t);
	virtual int set_mp_mmapsize(size_t);
	virtual int set_shm_key(long);
	virtual int set_timeout(db_timeout_t, u_int32_t);
	virtual int set_tmp_dir(const char *);
	virtual int set_tas_spins(u_int32_t);
	virtual int set_tx_max(u_int32_t);
	virtual int set_verbose(u_int32_t which, int);

	// Version information.  A static method so it can be obtained anytime.
	//
	static char *version(int *major, int *minor, int *patch);

	// Convert DB errors to strings
	static char *strerror(int);

	// Transaction functions
	//
%extend {
	DbTxn *txn_begin(DbTxn *pid, u_int32_t flags) {
		DbTxn *txn;
		self->txn_begin(pid, &txn, flags);
		return txn;
	}
}
	virtual int txn_checkpoint(u_int32_t kbyte, u_int32_t min,
			u_int32_t flags);
};
