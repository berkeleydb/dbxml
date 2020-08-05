<?php
$book_name = 'book1';
$book_content = "<book><title>Knowledge Discovery in Databases.</title></book>";
$container_name = 'test.dbxml';

$mgr = new XmlManager(null);
if(file_exists("test.dbxml")) {
    $mgr->removeContainer("test.dbxml");
}
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
