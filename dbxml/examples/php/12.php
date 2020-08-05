Currently non-functional.
<?php
$book_name = 'book1';
$book_content = '<book><title>Knowledge Discovery in Databases.</title></book>';
class myResolver extends XmlResolver {
    function resolveDocument($uri, &$result)
    {
        print "In resolveDocument($uri)\n";
        $result = new XmlValue("<a><b>b</b></a>");
        return true;
    }
    function resolveCollection($uri, &$result)
    {
        print "In resolveCollection($uri)\n";
        return false;
    }
}

$r = new myResolver();
$mgr = new XmlManager();
$mgr->registerResolver($r);
$con = $mgr->createContainer("test.dbxml");
$con->putDocument($book_name, $book_content);
$results = $mgr->query("doc('myscheme:xxx')/root");
print_r($results);
