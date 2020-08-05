Create an use a transactional XML Container:
add a document, get the document, display the content of
the document.

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
$con = $mgr->createContainer("test.dbxml");
$con->putDocument($book_name, $book_content);
$doc = $con->getDocument($book_name);
$s = $doc->getContentAsString();
print $doc->getName(). " = $s\n";
unset($doc);
unset($con);
$mgr->removeContainer("test.dbxml");
$env->close();
