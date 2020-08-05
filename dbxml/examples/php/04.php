Create an XmlContainer, define an equality string index
for booktitle elements, add a document, query the container
for the document, iterate over the result set displaying
the values returned.

EXPECTED RESULT:
book1 = <book><title>Knowledge Discovery in Databases.</title></book>
ACTUAL RESULT:
<?php
$book_name = 'book1';
$book_content = "<book><title>Knowledge Discovery in Databases.</title></book>";
$container_name = 'test.dbxml';

unlink($container_name);
$mgr = new XmlManager(null);
$con = $mgr->createContainer("test.dbxml");
$con->addIndex("", "title", "node-element-equality-string");
$con->putDocument($book_name, $book_content);

$results = $mgr->query("collection('test.dbxml')//*[title='Knowledge Discovery in Databases.']");

$results->reset();
while($results->hasNext()) 
{
    $val = $results->next();
    $doc = $val->asDocument();
    print $doc->getName()." = ".$val->asString()."\n";
}
