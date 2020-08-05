Create an XmlContainer, define an equality string index
for booktitle elements, add a document, create a query
context, define a variable binding, query the container
for the document within a context referencing the variable
defined, iterate over the result set displaying the values
returned.

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
$qc = $mgr->createQueryContext();
$qc->setVariableValue("title", "Knowledge Discovery in Databases.");
$results = $mgr->query("collection('test.dbxml')//*[title=\$title]", $qc);

$results->reset();
while($results->hasNext()) 
{
    $val = $results->next();
    $doc = $val->asDocument();
    print $doc->getName()." = ".$val->asString()."\n";
}
