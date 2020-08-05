/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2004,2009 Oracle. All rights reserved.
 *
 *
 *******
 * External function
 *
 * A very simple Berkeley DB XML program that demonstrates
 * external function API usage.
 *
 * This program demonstrates:
 *  XmlResolver used to resolve multiple external functions
 *  XmlExternalFunction implementations
 *  External function results handling
 *
 * MyFunResolver implements the XmlResolver interface, it is used to
 * resolve 2 distinct external function classes -- MyExternalFunctionPow
 * and MyExternalFunctionSqrt.  Instances of XmlExternalFunction must
 * always be "deleted" in order to release native memory they consume.
 *
 * MyExternalFunctionPow implements the XmlExternalFunction interface, and
 * implement the functionality of pow() function.
 *
 * MyExternalFunctionSqrt implements the XmlExternalFunction interface, and
 * implement the functionality of sqrt() function.
 *
 * In this example MyFunResolver will always
 * return new instance of MyExternalFunctionSqrt but will reuse a single
 * instance of MyExternalFunctionPow.  This means that the implementation
 * of MyExternalFunctionSqrt  must call "delete()" in its close function
 * in order to release native memory.  In the case of MyExternalFunctionPow,
 * MyFunResolver manages the instance which means it is responsible for
 * deleting the object and MyExternalFunctionPow.close() is a no-op.
 *
 * The choice of memory management scheme to use is up to the implementor of the
 * resolver and external function classes.
 *
 */
package misc;

import java.io.*;
import java.lang.Math.*;

import com.sleepycat.dbxml.*;

/* External function pow() implementation */
class MyExternalFunctionPow extends XmlExternalFunction {

    public XmlResults execute(XmlTransaction txn,
                              XmlManager mgr,
                              XmlArguments args)
        throws XmlException {

        // Retrieve argument as XmlValue
        XmlResults argResult1 = args.getArgument(0);
        XmlResults argResult2 = args.getArgument(1);

        XmlValue arg1 = argResult1.next();
        XmlValue arg2 = argResult2.next();

        // Call pow()
        double result = Math.pow(arg1.asNumber(), arg2.asNumber());

        // Create an XmlResults for return
        XmlResults results = mgr.createResults();
        XmlValue va = new XmlValue(result);
        results.add(va);

        return results;
    }

    // The base class's close routine will call delete() by default.
    // In order to reuse the object it's necessary to override that with
    // a no-op version of close().
    public void close() {
    }
}

/* External function sqrt() implementation */
class MyExternalFunctionSqrt extends XmlExternalFunction {

    public XmlResults execute(XmlTransaction txn,
                              XmlManager mgr,
                              XmlArguments args)
        throws XmlException {

        XmlResults argResult1 = args.getArgument(0);
        XmlValue arg1 = argResult1.next();

        // Call sqrt()
        double result = Math.sqrt(arg1.asNumber());

        XmlResults results = mgr.createResults();
        XmlValue va = new XmlValue(result);
        results.add(va);

        return results;
    }

    // MyFunResolver returns a new instance of this class on each invocation
    // so delete it here. The base class close() routine does this but
    // the overidded close() has to call delete() explicitly.
    public void close() {
        delete();
    }
}

/*
 * MyFunResolver returns a new instance of this object for each Resolution, so
 * that instance must be deleted here
 */
class MyFunResolver extends XmlResolver
{
    private String uri_ = "my://my.fun.resolver";
    XmlExternalFunction funPow = null;

    /*
     * Returns a new instance of either MyExternalFunctionPow or
     * MyExternalFuncitonSqrt if the URI, function name, and number of
     * arguments match.
     */
    public XmlExternalFunction resolveExternalFunction(XmlTransaction txn,
                                                       XmlManager mgr,
                                                       String uri,
                                                       String name,
                                                       int numberOfArgs)
        throws XmlException {

        XmlExternalFunction fun = null;
        if (uri.equals(uri_) && name.equals("pow") && (numberOfArgs == 2)) {
            // MyExternalFunctionPow is reusable.
            if(funPow == null)
                funPow = new MyExternalFunctionPow();
            return funPow;
        } else if (uri.equals(uri_) && name.equals("sqrt") && (numberOfArgs == 1)) {
            // MyExternalFunctionSqrt is unreusable.
            fun = new MyExternalFunctionSqrt();
        }

        return fun;
    }

    public String getUri(){
        return uri_;
    }

    public void close(){
        if(funPow != null)
            funPow.delete();
    }
};

public class ExternalFunction {

    public static void main(String[] args)
        throws Exception{

        try {
        // Create an XmlManager
        XmlManager mgr = new XmlManager();

        // Create an function resolver
        MyFunResolver resolver = new MyFunResolver();

        // Register the function resolver to XmlManager
        mgr.registerResolver(resolver);

        XmlQueryContext context = mgr.createQueryContext();

        // Set the prefix URI
        context.setNamespace("my", resolver.getUri());

        for(int i=1; i<=100; i++) {
            // Query that calls the external function pow()
            // The function must be declared in the query's preamble
            String query1 = "declare function " +
                "my:pow($a as xs:double, $b as xs:double) as xs:double external;\n" +
                "my:pow(" + i + "," + 3 + ")";

            // Query call external sqrt() function
            String query2 = "declare function " +
                "my:sqrt($a as xs:double) as xs:double external;\n" +
                "my:sqrt(" + i + ")";

            XmlResults results = mgr.query(query1, context);

            // The first query returns the result of pow(i,3)
            while (results.hasNext()) {
                XmlValue va = results.next();
                String out = i + "^3 = " + va.asNumber();
                System.out.println(out);
            }

            // The second query returns the result of sqrt(i)
            results = mgr.query(query2, context);
            while (results.hasNext()) {
                XmlValue va = results.next();
                String out = "Sqrt(" + i + ") = " + va.asNumber();
                System.out.println(out);
            }
        }

        // If the resolver reuse XmlExternalFunction objects, it is responsible for
        // eventually calling delete()
        resolver.close();

        } catch (XmlException xe) {
            String out = "XmlException: " + xe.getMessage() + "\n";
            System.out.println(out);
        }
    }
}
