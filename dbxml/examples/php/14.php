Displays the behavior of XmlInputStream.  When a is stream is created it can
only be destroyed by passing it to putDocument.  Also, once it is passed
to putDocument, if it is accessed again it will cause a segment fault

EXPECTED RESULT:
book1.xml = <book><title>Knowledge Discovery in Databases.</title></book>
ACTUAL RESULT:
<?php
$file_name = "book1.xml";

$mgr = new XmlManager();
if ($mgr->existsContainer("test.dbxml")) {
   $mgr->removeContainer("test.dbxml");	
}
$con = $mgr->createContainer("test.dbxml");
$inputstream = $mgr->createLocalFileInputStream($file_name);
$con->putDocument($file_name, $inputstream);
$doc = $con->getDocument($file_name);
$s = $doc->getContentAsString();
print $doc->getName(). " = $s\n";
unset($doc);
unset($con);
$mgr->removeContainer("test.dbxml");
