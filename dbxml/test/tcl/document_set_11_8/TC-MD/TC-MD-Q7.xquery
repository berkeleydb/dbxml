for $a in collection("11.8.3.dbxml")/article
where every $b in $a/body/abstract/p satisfies
    contains($b, "hockey")
return
    $a/prolog/title
