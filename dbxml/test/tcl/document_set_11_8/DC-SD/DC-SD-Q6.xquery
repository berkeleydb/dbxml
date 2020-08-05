for $item in collection("11.8.2.dbxml")/catalog/item
where some $auth in
    $item/authors/author/contact_information/mailing_address
satisfies $auth/name_of_country = "Canada"
return
    $item
