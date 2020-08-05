Display indexes for a container.

EXPECTED RESULT:
array(3) {
  ["uri"]=>
  string(35) "http://www.sleepycat.com/2002/dbxml"
  ["name"]=>
  string(4) "name"
  ["index"]=>
  string(36) "unique-node-metadata-equality-string"
}
array(3) {
  ["uri"]=>
  string(0) ""
  ["name"]=>
  string(5) "title"
  ["index"]=>
  string(57) "node-element-equality-string edge-element-equality-string"
}
ACTUAL RESULT:
<?php
$mgr = new XmlManager();
if(file_exists("test.dbxml")) {
  $mgr->removeContainer("test.dbxml");
}
$con = $mgr->createContainer("test.dbxml");
$con->addIndex("", "title", "node-element-equality-string, edge-element-equality-string");
$foo = $con->getIndexSpecification();
foreach($foo->getIndexes() as $a) {
    var_dump($a);
}
