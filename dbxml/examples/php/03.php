Create an XmlContainer, add a document that includes a
namespace definition, create a query context, define a
namespace prefix to URI mapping, query the container
for the document within a context, iterate over the
result set displaying the values returned.

EXPECTED RESULT:
book1_ns = <book xmlns:books="http://foo.bar.com/books.dtd"><books:title>Knowledge Discovery in Databases.</books:title></book>
ACTUAL RESULT:
<?php
$book_name = 'book1_ns';
$book_content = "<book xmlns:books='http://foo.bar.com/books.dtd'><books:title>Knowledge Discovery in Databases.</books:title></book>";
$container_name = 'test.dbxml';

$mgr = new XmlManager(null);
if(file_exists("test.dbxml")) {
    $mgr->removeContainer("test.dbxml");
}
$con = $mgr->createContainer("test.dbxml");
$con->putDocument($book_name, $book_content);

$qc = $mgr->createQueryContext();
$qc->setNamespace("books2", "http://foo.bar.com/books.dtd");

$results = $mgr->query("collection('test.dbxml')/*[books2:title='Knowledge Discovery in Databases.']", $qc);

$results->reset();
while($results->hasNext()) 
{
    $val = $results->next();
    $doc = $val->asDocument();
    print $doc->getName()." = ".$val->asString()."\n";
}
