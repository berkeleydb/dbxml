for $a in collection("11.8.2.dbxml")/catalog/item
where $a/@id="I5"
return
    $a//ISBN/text()
