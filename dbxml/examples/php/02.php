Create an XmlManager/XmlContainer, add a document, query the container
for the document, iterate over the result set displaying the
values returned.

EXPECTED RESULT:
book1 = <book><title>Knowledge Discovery in Databases.</title></book>
ACTUAL RESULT:
<?php
$book_name = 'book1';
$book_content = '<book><title>Knowledge Discovery in Databases.</title></book>';
$container_name = 'test.dbxml';

$mgr = new XmlManager();
if(file_exists("test.dbxml")) {
    $mgr->removeContainer("test.dbxml");
}
$con = $mgr->createContainer("test.dbxml");
$con->putDocument($book_name, $book_content);
$results = $mgr->query("collection('test.dbxml')/book");

$results->reset();
while($results->hasNext()) 
{
    $val = $results->next();
    $doc = $val->asDocument();
    print $doc->getName()." = ".$val->asString()."\n";
}
