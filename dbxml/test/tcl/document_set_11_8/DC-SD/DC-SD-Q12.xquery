for $a in collection("11.8.2.dbxml")/catalog/item[@id="I6"]
return
    <Output>
        {$a/authors/author[1]/contact_information/mailing_address}
    </Output>
