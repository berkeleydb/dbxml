for $a in collection("11.8.2.dbxml")/catalog/item[@id="I3"]
return
    $a/authors/author[1]
