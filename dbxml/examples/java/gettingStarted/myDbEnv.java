//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2009 Oracle.  All rights reserved.
//
//

package dbxml.gettingStarted;

import java.io.*;
import com.sleepycat.db.*;
import com.sleepycat.dbxml.*;

//Class used to open and close a Berkeley DB environment
public class myDbEnv
{
    private Environment dbEnv_ = null;
    private XmlManager mgr_ = null;
    private File path2DbEnv_ = null;

    public myDbEnv(File path2DbEnv) 
	throws Throwable {
	if (! path2DbEnv.isDirectory()) {
	    throw new Exception(path2DbEnv.getPath() + 
				" does not exist or is not a directory.");
	}

        EnvironmentConfig config = new EnvironmentConfig();
        config.setCacheSize(50 * 1024 * 1024); // 50MB
        config.setAllowCreate(true);
        config.setInitializeCache(true);
        config.setTransactional(true);
        config.setInitializeLocking(true);
	config.setInitializeLogging(true);
	config.setErrorStream(System.err);
        dbEnv_ = new Environment(path2DbEnv, config);

	path2DbEnv_ = path2DbEnv;
	mgr_ = new XmlManager(dbEnv_, null);
    }

    //Returns the path to the database environment
    public File getDbEnvPath() { return path2DbEnv_; }

    //Returns the database environment encapsulated by this class.
    public Environment getEnvironment() { return dbEnv_; }

    //Returns the XmlManager encapsulated by this class.
    public XmlManager getManager() { return mgr_; }

    //Used to close the environment
    public void cleanup() throws DatabaseException
    {
       if (mgr_ != null) {
	   mgr_.delete();
	   mgr_ = null;
       }
       if (dbEnv_ != null) {
	   dbEnv_.close();
	   dbEnv_ = null;
       }

    }
}
