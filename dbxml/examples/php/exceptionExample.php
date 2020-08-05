EXPECTED RESULT:
book1 = <book><title>Knowledge Discovery in Databases.</title></book>
ACTUAL RESULT:
<?php
#
# This is a very simple example of using try/catch and XmlException
# to get information on an exception thrown from Berkeley DB XML.
# The line that generates the exception is the query, which uses
# bad XQuery syntax.
#
$book_name = 'book1';
$book_content = '<book><title>Knowledge Discovery in Databases.</title></book>';
$mgr = new XmlManager();
try {
	$con = $mgr->createContainer("test.dbxml");
	$con->putDocument($book_name, $book_content);
	$qc = $mgr->createQueryContext();
	$res = $mgr->query("collection('test.dbxml')/x[", $qc);
	$doc = $con->getDocument("foo");
	$s = $doc->getContentAsString();
	print $doc->getName(). " = $s\n";
	unset($doc);
	unset($con);
} catch (XmlException $xe) {
	print "XmlException message: ".$xe->what()."\n";
	print "XmlException code: ".$xe->getExceptionCode()."\n";
	print "XmlException dbErrno: ".$xe->getDbErrno()."\n";
	print "Query line: ". $xe->getQueryLine()."\n";
	print "Query column: ". $xe->getQueryColumn()."\n";
	unset($con);
}
$mgr->removeContainer("test.dbxml");
