//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2003,2009 Oracle.  All rights reserved.
//
//
package com.sleepycat.dbxml.perftest;

import com.sleepycat.db.*;
import com.sleepycat.dbxml.*;
import java.io.File;
import java.io.FileInputStream;
import java.util.*;

//
// Create a container with various index configurations and measure the throughput
// of queries and updates.
//
public class PerfTest
{
	public static void main(String[] args) throws Exception
	{
		String config_file = null;
		String home = "TESTDIR";
		int iterations = 0;
		boolean cleanup = false;
		boolean initonly = false;
		boolean recover = false;

		for (int i = 0; i < args.length; i++) {
			if ("-c".equals(args[i]) && i + 1 < args.length) {
				config_file = args[++i];
			} else if("-C".equals(args[i])) {
				cleanup = true;
			} else if("-h".equals(args[i]) && i + 1 < args.length) {
				home = args[++i];
			} else if("-i".equals(args[i])) {
				initonly = true;
			} else if("-n".equals(args[i]) && i + 1 < args.length) {
				iterations = Integer.parseInt(args[++i]);
			} else if("-r".equals(args[i])) {
				recover = true;
			} else {
				System.err.println("Unknown arg: " + args[i]);
				usage();
				System.exit(1);
			}
		}

		if (config_file == null) {
			usage();
			System.exit(1);
		}

		PerfTest test = new PerfTest(config_file, home, iterations);
		if (cleanup)
			test.cleanup();
		test.initialize(recover);
		if (!initonly) {
			test.start();
			test.monitor();

			// Now shut it down
			finished = true;
			test.end();
		}
	}

	static void usage() {
		System.out.println("PerfTest - runs performance tests against DB XML");
		System.out.println("Usage:");
		System.out.println("\tjava [-Dprop=override] " + PerfTest.class.getName() + " -c config [-C] [-h home] [-i] [-n iterations] [-r]");
		System.out.println("\t\t[-Dprop=override]\toverride settings in the config file");
		System.out.println("\t\t-c config\ta properties file that describes the test");
		System.out.println("\t\t-C\t\tcleanup before starting");
		System.out.println("\t\t-h home\t\tuse a home directory [TESTDIR]");
		System.out.println("\t\t-n iterations\tstop after a number of iterations");
		System.out.println("\t\t-r\t\trun recovery on the environment before starting");
	}

	PerfTest(String config_file, String home, int iterations) throws Exception {
		this.home = home;
		this.iterations = iterations;

		// Load configuration file
		Properties config = new Properties();
		config.load(new FileInputStream(config_file));

		// Apply any command line overrides
		config.putAll(System.getProperties());

		buildTest(config);
	}

	void buildTest(Properties config) throws Exception {
		// Containers
		for (int nc = 1; ; nc++) {
			String cprefix = "container" + nc;
			String cname = config.getProperty(cprefix + ".name");
			if (cname == null)
				break;

			TestContainer tc = new TestContainer(cname);
			containers.add(tc);

			for (int ni = 1; ; ni++) {
				String iprefix = cprefix + ".index" + ni;
				String inode = config.getProperty(iprefix + ".node");
				if (inode == null)
					break;

				String iurl = config.getProperty(iprefix + ".url", "");
				String itype = config.getProperty(iprefix + ".type");
				tc.addIndex(new XmlIndexDeclaration(iurl, inode, itype));
			}

			for (int nd = 1; ; nd++) {
				String dprefix = cprefix + ".doctype" + nd;
				String dfragment = config.getProperty(dprefix + ".fragment");
				if (dfragment == null)
					break;

				int dsize = Integer.parseInt(config.getProperty(dprefix + ".size"));
				tc.addDocType(new DocumentCreator(dfragment, dsize));
			}
		}

		// Transaction types
		for (int nt = 1; ; nt++) {
			String tprefix = "txn_type" + nt;
			String ops = config.getProperty(tprefix);
			if (ops == null)
				break;

			TransactionType tt = new TransactionType();
			txntypes.add(tt);

			// Split up whitespace-separated operations
			for (StringTokenizer tok = new StringTokenizer(ops); tok.hasMoreTokens(); ) {
				String op = tok.nextToken();
				// Split up container<num>.<op>(<index>)
				int dot = op.indexOf('.');
				int bracket = op.indexOf('(');
				int cindex = Integer.parseInt(op.substring("container".length(), dot));
				String optype = op.substring(dot + 1, bracket);
				String args = op.substring(bracket + 1, op.length() - 1);

				TestContainer container = (TestContainer)containers.elementAt(cindex);
				tt.addOp(Operation.create(container, optype, args));
			}
		}

		// Thread types
		for (int nt = 1; ; nt++) {
			String tprefix = "thread_type" + nt;
			String txns = config.getProperty(tprefix);
			if (txns == null)
				break;
			int tcount = Integer.parseInt(config.getProperty(tprefix + ".count", "1"));

			ThreadType tt = new ThreadType(tcount);
			threadtypes.add(tt);

			// Split up whitespace-separated operations
			for (StringTokenizer tok = new StringTokenizer(txns); tok.hasMoreTokens(); ) {
				int txn_index = Integer.parseInt(tok.nextToken());
				tt.addTxn((TransactionType)txntypes.get(txn_index));
			}
		}
	}

	void cleanup() throws Exception {
		System.out.println("*** cleanup (" + home + ") ***");

		File dir = new File(home);
		File[] contents = dir.listFiles();
		if (contents != null) {
			for (int i = 0; i < contents.length; i++)
				contents[i].delete();
			dir.delete();
		}
		dir.mkdirs();
	}

	void initialize(boolean recover) throws Exception {
		System.out.println("*** initialize (" + (recover ? "with" : "without") + " recovery) ***");

		PerfTest.env = new DbEnv(0);
		env.open(home, Db.DB_CREATE |
		    Db.DB_INIT_LOCK | Db.DB_INIT_LOG | Db.DB_INIT_MPOOL | Db.DB_INIT_TXN, 0);
		DbTxn txn = env.txn_begin(null, 0);

		for (int i = 1; i < containers.size(); i++)
			((TestContainer)containers.get(i)).create(env, txn, 0);

		txn.commit(0);
	}

	void start() throws Exception {
		System.out.println("*** start ***");
		for (int i = 1; i < threadtypes.size(); i++)
			((ThreadType)threadtypes.get(i)).start();
	}

	void monitor() throws Exception {
		long startTick = System.currentTimeMillis();
		long lastTick = startTick;
		for (int seconds = 0; seconds < 60; seconds++) {
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) { /* ignore */ }

			long now = System.currentTimeMillis();
			for (int i = 1; i < threadtypes.size(); i++) {
				long iterations = ((ThreadType)threadtypes.get(i)).countIterations();
				double rate = 1000.0 * iterations / (now - startTick);
				System.out.println("Thread " + i + ": total txns: " + iterations + " @ " + rate + " / second");
			}
			lastTick = now;
		}
	}

	void end() throws Exception {
		for (int i = 1; i < threadtypes.size(); i++)
			((ThreadType)threadtypes.get(i)).join();

		for (int i = 1; i < containers.size(); i++)
			((TestContainer)containers.get(i)).close(0);

		env.close(0);
	}

	static class TestContainer {
		public TestContainer(String name) {
			this.name = name;
		}

		public void addDocType(DocumentFactory dt) {
			doctypes.add(dt);
		}

		public DocumentFactory getDocType(int index) {
			return (DocumentFactory)doctypes.get(index);
		}

		public void addIndex(XmlIndexDeclaration id) {
			indexes.add(id);
		}

		public void create(DbEnv env, DbTxn txn, int flags) throws Exception {
			System.out.println("** creating container " + name + " **");
			container = new XmlContainer(env, name, flags);
			container.open(txn, Db.DB_CREATE | Db.DB_THREAD, 0);

			for (Enumeration i = indexes.elements(); i.hasMoreElements(); ) {
				XmlIndexDeclaration id = (XmlIndexDeclaration)i.nextElement();
				if (id != null) {
					System.out.println("* declaring index container (" + id.uri + ", " + id.name + ", " + id.index + ") *");
					container.addIndex(txn, id.uri, id.name, id.index);
				}
			}
		}

		public void close(int flags) throws Exception {
			container.close(0);
		}

		private String name;
		private Vector doctypes = new Vector(); { doctypes.add(null); }
		private Vector indexes = new Vector();
		public XmlContainer container;
	}

	static interface DocumentFactory {
		public XmlDocument getDocument() throws Exception;
	}

	static class DocumentCreator implements DocumentFactory {
		public DocumentCreator(String fragment, int size) {
			this.fragment = fragment;
			this.size = size;
		}

		public XmlDocument getDocument() throws Exception {
			StringBuffer buf = new StringBuffer();
			buf.append("<document>");
			while(buf.length() + 11 < size)
				buf.append(fragment);
			buf.append("</document>");
			XmlDocument doc = new XmlDocument();
			doc.setContent(buf.toString());
			return doc;
		}

		private String fragment;
		private int size;
	}

	static abstract class Operation {
		public static Operation create(TestContainer tc, String opname, String args) throws Exception {
			if ("put".equals(opname))
				return new PutOperation(tc, args);
			else
				throw new Exception("Unknown op: " + opname);
		}

		protected Operation(TestContainer tc) {
			this.tc = tc;
		}

		public abstract void execute(DbTxn txn) throws Exception;

		protected TestContainer tc;
	}

	static class PutOperation extends Operation {
		public PutOperation(TestContainer tc, String args) {
			super(tc);
			int dindex = Integer.parseInt(args);
			this.df = tc.getDocType(dindex);
		}

		public void execute(DbTxn txn) throws Exception {
			tc.container.putDocument(txn, df.getDocument(), null, 0);
		}

		private DocumentFactory df;
	}

	static class TransactionType {
		public void addOp(Operation op) {
			ops.add(op);
		}

		public void execute() throws Exception {
			DbTxn txn = PerfTest.env.txn_begin(null, 0);
			for (int i = 0; i < ops.size(); i++)
				((Operation)ops.get(i)).execute(txn);
			txn.commit(0);
		}

		private Vector ops = new Vector();
	}

	static class ThreadType {
		public ThreadType(int count) {
			this.workers = new Worker[count];
		}

		public void addTxn(TransactionType tt) {
			txns.add(tt);
		}

		public void start() {
			System.out.println("** Starting " + workers.length + " workers(s) **");
			for (int i = 0; i < workers.length; i++) {
				workers[i] = new Worker();
				workers[i].start();
			}
		}

		public void join() throws Exception {
			for (int i = 0; i < workers.length; i++)
				workers[i].join();
		}

		public long countIterations() {
			long total_iter = 0;
			for (int i = 0; i < workers.length; i++)
				total_iter += workers[i].niter;
			return total_iter;
		}

		class Worker extends Thread {
			public void run() {
				int ntxns = txns.size();
				try {
					for (; !PerfTest.finished; niter++) {
						int txntype = PerfTest.random.nextInt(ntxns);
						((TransactionType)txns.get(txntype)).execute();
					}
				} catch (Exception e) {
					e.printStackTrace(System.err);
				}
			}

			// Maintain per-thread counts so we're not synchronizing
			public long niter;
		}

		private PerfTest test;
		private Worker[] workers;
		private Vector txns = new Vector();
	}

	private String home;
	private int iterations;
	private Vector containers = new Vector(); { /* 1-based */ containers.add(null); }
	private Vector txntypes = new Vector(); { /* 1-based */ txntypes.add(null); }
	private Vector threadtypes = new Vector(); { /* 1-based */ threadtypes.add(null); }

	public static DbEnv env;
	public static boolean finished = false;
	public static Random random = new Random();
}
