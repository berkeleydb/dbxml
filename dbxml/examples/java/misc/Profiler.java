/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2004,2009 Oracle. All rights reserved.
 *
 *
 *******
 *
 * Simple profiler example
 * A very simple Berkeley DB XML query profiler based on debug API.
 * 
 * This program demonstrates how to set a custom debug listener on 
 * XmlQueryContext, and how to derive a class from XmlDebugListener.
 * The derived class, MySimpleProfiler, records execution time and 
 * frequency for expressions in the query executed.
 *
 * ExpressionProfile is used to represent an expression's profiling
 * information. If the expression is executed many times, ExpressionProfile
 * holds the sum execution time.
 *  
 */

package misc;

import java.util.Iterator;
import java.util.Map;
import java.util.TreeMap;


import com.sleepycat.dbxml.*;

class MySimpleProfiler extends XmlDebugListener {

	private Map<ExpressionProfile, ExpressionProfile> exprs_;
	private String query_;

	public MySimpleProfiler(String query) {
		query_ = query;
		exprs_ = new TreeMap<ExpressionProfile, ExpressionProfile>();
	}

	public void enter(XmlStackFrame stack) throws XmlException {

		ExpressionProfile ep = new ExpressionProfile(this, stack);
		
		if (exprs_.containsKey(ep)) {
			ep = exprs_.get(ep);
			ep.recordEnter();
		} else {
			ep = new ExpressionProfile(this, stack);
			ep.recordEnter();
			exprs_.put(ep, ep);
		}

	}

	public void exit(XmlStackFrame stack) throws XmlException {
		
		ExpressionProfile ep = new ExpressionProfile(this, stack);

		if (exprs_.containsKey(ep)) {
			ep = exprs_.get(ep);
			ep.recordExit();
		} else {
			throw new XmlException(XmlException.INVALID_VALUE,
					"Can not find enter clause!");
		}
		
	}

	public void getProfileInfo() {
		Iterator<Map.Entry<ExpressionProfile, ExpressionProfile>>  it  = exprs_.entrySet().iterator();
		while(it.hasNext()){
			Map.Entry<ExpressionProfile, ExpressionProfile> entry = it.next();
			entry.getValue().printProfileInfo();
		}

	}

	// Get the query expression by line and column
	public String getExprStr(int line, int column){
		
        int start=0, end=0;

        for (int i=0; i < line ; i++){
                start = end;          
                end = query_.indexOf("\n", start+1);
                if (end == start+1 || end == -1)              
                        end = query_.length();   
        }                                        

        end--;

        if (start != 0)
                start++;

        return query_.substring( start+column-1, end+1 );

	}
}

// To collect profile information for every expression
class ExpressionProfile implements java.lang.Comparable<ExpressionProfile>{
	
	static String format = "Expression: %-60s at line:%2s column:%2s execution time:%f s, executed %s times\n";
	
	// Use line and column to identify an expression
	private int line;
	private int column;

	private int executeTimes;

	private long startTime;
	private long totalTime;
	private String expr;
	private int depth;

	public ExpressionProfile(MySimpleProfiler profiler, XmlStackFrame stack) throws XmlException {
		executeTimes = 0;
		line = stack.getQueryLine();
		column = stack.getQueryColumn();
		expr = profiler.getExprStr(line, column);
		depth=0;
	}

	public void printProfileInfo() {
		
		double ttime = (double)totalTime;
		
		// nanoseconds to seconds
		ttime = ttime / 100000000;
		
		System.out.printf(format, expr, line, column, ttime, executeTimes);
	}

	public void recordEnter() {
		executeTimes++;
		if (depth == 0)
			startTime = System.nanoTime();
		depth++;
	}

	public void recordExit() {
		depth--;
		if( depth == 0 )
			totalTime = totalTime + System.nanoTime() - startTime;
	}

	public int compareTo(ExpressionProfile o) {
		int cmp = this.line - o.line;
		if (cmp != 0) return cmp; 
		return this.column-o.column;
	}
}

public class Profiler {

	public static void main(String[] args) throws Throwable {
		
		String containerName = "profilerContainer.dbxml";
		String docName = "test.xml";
		String docString = "<root><a>1</a><a>2</a><a>3</a></root>";
		String query = "for $a in collection('profilerContainer.dbxml')/root/a\n"
				 	 + "where $a>'2'\n" 
				 	 + "return $a";

		XmlManager mgr = new XmlManager();
		XmlContainer cont = null;
		XmlResults re = null;
		XmlQueryExpression expr = null;
		
		try {

			if (mgr.existsContainer(containerName) != 0)
				mgr.removeContainer(containerName);
			
			XmlContainerConfig contConf = new XmlContainerConfig();
			cont = mgr.createContainer(containerName, contConf);

			XmlUpdateContext uc = mgr.createUpdateContext();
			cont.putDocument(docName, docString, uc);

			MySimpleProfiler dl = new MySimpleProfiler(query);
			XmlQueryContext qc = mgr.createQueryContext();
			qc.setDebugListener(dl);
			
			expr = mgr.prepare(query, qc);
			re = expr.execute(qc);
			re.next();
			
			System.out.printf("Simple profiling of query:\n%s \n\n", query);
			dl.getProfileInfo();
			
		} catch (XmlException e) {
			System.out.println("Exception: " + e.getMessage());
		} finally {
			// Clean up
			expr.delete();
			re.delete();
			
			cont.delete();
			mgr.delete();		
		}
	}

}
