for $prolog in collection("11.8.3.dbxml")/article/prolog
where
    $prolog/authors/author/name="Ben Yang"
return
    $prolog/title
