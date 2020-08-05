Create an XML Container within a Berkeley DB environment,
then within a Berkeley DB transaction, add a document,
get the document, display the content of the document.
Use a Berkeley DB transaction.

EXPECTED RESULT:
book1 = <book><title>Knowledge Discovery in Databases.</title></book>
ACTUAL RESULT:
<?php

$book_name = 'book1';
$book_content = '<book><title>Knowledge Discovery in Databases.</title></book>';

foreach(array_merge(glob("__db*"), glob("log.O"), glob("test*.dbxml")) as $file)
{
    @unlink($file);
}

$env = new Db4Env();
$env->open();
$mgr = new XmlManager($env);
$config = new XmlContainerConfig();
$config->setTransactional(true);
$con = $mgr->createContainer("test.dbxml", $config);
$txn = $mgr->createTransaction();
$doc = $mgr->createDocument();
$doc->setContent($book_content);
$doc->setName($book_name);
$con->putDocument($txn, $doc);
$doc = $con->getDocument($txn, $book_name);
$s = $doc->getContentAsString();
print $doc->getName(). " = $s\n";
$txn->commit();
unset($doc);
unset($con);
$mgr->removeContainer("test.dbxml");
$env->close();
