Shows how XmlContainerConfig can be used to set the properties of 
a container.  In this case the property is set so that the container 
will be created on open if it does not exist.

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

$config = new XmlContainerConfig();
$config->setAllowCreate(true);
if ($config->getAllowCreate()) {
  $con = $mgr->openContainer("test.dbxml", $config);
  $con->putDocument($book_name, $book_content);
  $doc = $con->getDocument($book_name);
  $s = $doc->getContentAsString();
  print $doc->getName(). " = $s\n";
  unset($doc);
  unset($con);
  $mgr->removeContainer("test.dbxml");
}
