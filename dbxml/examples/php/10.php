Create two XML Containers within a Berkeley DB environment,
then within a Berkeley DB transaction add a document to
each container.

EXPECTED RESULT:
Success
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
$mgr->setDefaultContainerConfig($config);
$con1 = $mgr->createContainer("test.dbxml");
$con2 = $mgr->createContainer("test2.dbxml");
$txn = $mgr->createTransaction();
$con1->putDocument($txn, $book_name, $book_content);
$con2->putDocument($txn, $book_name, $book_content);
$txn->commit();
unset($con1);
unset($con2);
unset($mgr);
$env->close();
print "Success\n";
