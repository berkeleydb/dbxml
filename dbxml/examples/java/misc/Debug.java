/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2004,2009 Oracle. All rights reserved.
 *
 *
 *******
 *
 * Debug API
 * A very simple Berkeley DB XML program that demonstrates
 * debug API usage.
 * 
 * This program demonstrates how to set a custom debug listener 
 * class on the XmlQueryContext, and how to derive a class form 
 * XmlDebugListener. The derived class, MyDebugListener, simply 
 * prints out the execution information when a query is executed.
 *   
 */

package misc;

import com.sleepycat.dbxml.*;

class MyDebugListener extends XmlDebugListener {

	private int depth_ = -1;
	private String outputFormat = "%s%s:\"%s\" at %s:%s:%s\n";
	private String query_;
	
	public MyDebugListener(String query){
		query_ = query;
	}
	
	public void start(XmlStackFrame stack) throws XmlException {
		depth_++;
		System.out.printf(outputFormat, getIndentation(depth_), "{START}", getExprStr(stack), stack.getQueryFile(), stack.getQueryLine(),stack.getQueryColumn());
	}

	public void end(XmlStackFrame stack) throws XmlException {
		System.out.printf(outputFormat, getIndentation(depth_), "{END}", getExprStr(stack), stack.getQueryFile(), stack.getQueryLine(),stack.getQueryColumn());
		depth_--;
	}

	public void enter(XmlStackFrame stack) throws XmlException {
		depth_++;
		System.out.printf(outputFormat, getIndentation(depth_), "[ENTER]", getExprStr(stack), stack.getQueryFile(), stack.getQueryLine(),stack.getQueryColumn());
	}

	public void exit(XmlStackFrame stack) throws XmlException {
		System.out.printf(outputFormat, getIndentation(depth_), "[EXIT]", getExprStr(stack), stack.getQueryFile(), stack.getQueryLine(),stack.getQueryColumn());
		depth_--;
	}

	public void error(XmlException error, XmlStackFrame stack)
			throws XmlException {
		System.out.printf("An error occurred in expression. line:%s column:%s\n", stack.getQueryLine(),stack.getQueryColumn());
        throw error;
	}

	// Function return indentation spaces 	
	private String getIndentation(int depth) {
		StringBuffer sb = new StringBuffer("");
		
		for(int i=0; i<depth; i++)
			sb.append("    ");
		
		return sb.toString();
	}
	
	// Get the query expression by line and column
	// Cut the string starting form column number to the 
	// end of the line 
	private String getExprStr(XmlStackFrame stack) throws XmlException{

		int line = stack.getQueryLine();
		int column = stack.getQueryColumn();
		
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


public class Debug {

	public static void main(String[] args) throws Throwable {
		
		String containerName = "debugContainer.dbxml";
		String docName = "test.xml";
		String docString = "<root><a>1</a><a>2</a><a>3</a><a>4</a></root>";
		
		String query = "for $a in collection('debugContainer.dbxml')/root/a\n"
		 	 			+ "where $a>'2'\n" 
		 	 			+ "return $a";
		
		XmlManager mgr = new XmlManager();
		XmlContainer cont = null;
		XmlResults re = null;
		XmlQueryExpression expr = null;
		
		try {

			// Because the container will exist on disk, remove it
            // first if exist
			if (mgr.existsContainer(containerName) != 0)
				mgr.removeContainer(containerName);
			
			//create container
			cont = mgr.createContainer(containerName);
			XmlIndexSpecification is = cont.getIndexSpecification();
			is.setAutoIndexing(false);
			cont.setIndexSpecification(is);

			XmlUpdateContext uc = mgr.createUpdateContext();
			cont.putDocument(docName, docString, uc);

			// Set the custom debug listener on XmlQueryContext
			MyDebugListener dl = new MyDebugListener(query);
			XmlQueryContext qc = mgr.createQueryContext();
			qc.setEvaluationType(XmlQueryContext.Eager);
			qc.setDebugListener(dl);
			
			expr = mgr.prepare(query, qc);
			System.out.println("Query: ");
			System.out.println(query);
			System.out.println("\nExecution process:\n");
			
			re = expr.execute(qc);

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

