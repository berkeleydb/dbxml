Create an XML Container, rename that container, delete the
container, and repeat.
EXPECTED RESULT:
Success
ACTUAL RESULT:
<?php

$mgr = new XmlManager();
for($i=0; $i < 5; $i++) {
    $con = $mgr->createContainer("test1.dbxml");
    unset($con);
    $mgr->renameContainer("test1.dbxml", "test2.dbxml");
    $con = $mgr->openContainer("test2.dbxml");
    unset($con);
    $mgr->removeContainer("test2.dbxml");
}
print "Success\n";
