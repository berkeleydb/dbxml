for $art in collection("11.8.3.dbxml")/article[@id="2"]
return
    $art/prolog/*/author/name
