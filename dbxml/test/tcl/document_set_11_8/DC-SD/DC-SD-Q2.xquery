for $item in collection("11.8.2.dbxml")/catalog/item
where $item/authors/author/name/first_name = "Ben"
return
    $item/title
