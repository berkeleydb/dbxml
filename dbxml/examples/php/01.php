EXPECTED RESULT:
book1 = <book><title>Knowledge Discovery in Databases.</title></book>
ACTUAL RESULT:
<?php
$book_name = 'book1';
$book_content = '<book><title>Knowledge Discovery in Databases.</title></book>';
$mgr = new XmlManager();
if ($mgr->existsContainer("test.dbxml")) {
   $mgr->removeContainer("test.dbxml");	
}
$con = $mgr->createContainer("test.dbxml");
$con->putDocument($book_name, $book_content);
$doc = $con->getDocument($book_name);
$s = $doc->getContentAsString();
print $doc->getName(). " = $s\n";
unset($doc);
unset($con);
$mgr->removeContainer("test.dbxml");
