for $a in collection("11.8.3.dbxml")/article
where some $b in $a/body/abstract/p satisfies
    (contains($b, "the") and contains($b, "hockey"))
return
    $a/prolog/title
