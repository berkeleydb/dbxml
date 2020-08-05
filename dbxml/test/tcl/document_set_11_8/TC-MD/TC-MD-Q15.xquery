for $a in collection("11.8.3.dbxml")/article/prolog/authors/author
where empty($a/contact/text())
return
    <NoContact>
        {$a/name}
    </NoContact>
