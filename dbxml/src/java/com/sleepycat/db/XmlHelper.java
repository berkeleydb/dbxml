//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

// This class allows dipping into package-protected state
// in Berkeley DB.  Ideally, there would be a better way.
// That is TBD.

package com.sleepycat.db;
import com.sleepycat.db.internal.DbEnv;
import com.sleepycat.db.internal.DbTxn;
import com.sleepycat.db.LockMode;
import com.sleepycat.dbxml.XmlException;

public class XmlHelper {

    public static DbEnv getDbEnv(Environment env) 
    throws XmlException {
	try{
	    env.getHome();
	}catch(DatabaseException e){
	    throw (new XmlException(XmlException.DATABASE_ERROR, 
		   "Invalid Environment object, may be closed.", 
		    e, e.getErrno()));
	}catch(Exception e){
	    throw (new XmlException(XmlException.DATABASE_ERROR, 
		   "Invalid Environment object, may be closed.", 
		   new DatabaseException("Invalid environment."), 0));
	}
	return env.unwrap();
    }
    public static DbTxn getDbTxn(Transaction txn) {
	return txn.txn;
    }
    public static Environment makeNewEnvironment(DbEnv env) 
    throws DatabaseException {
	return new Environment(env);
    }
    public static int getFlag(LockMode mode)
    throws DatabaseException {
	return LockMode.getFlag(mode);
    }
};
