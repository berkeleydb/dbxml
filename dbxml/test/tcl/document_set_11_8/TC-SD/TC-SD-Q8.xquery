for $ent in collection("11.8.4.dbxml")/dictionary/e
where $ent/*/hw = "and"
return
    $ent/ss/s/qp/*/qt
