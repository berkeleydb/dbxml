for $item in collection("11.8.2.dbxml")/catalog/item
where every $add in
    $item/authors/author/contact_information/mailing_address
satisfies $add/name_of_country = "Canada"
return
    $item
