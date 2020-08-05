for $a in collection("11.8.3.dbxml")/article
where some $p in $a//p satisfies contains($p, "hockey")
return
    $a/prolog/title
