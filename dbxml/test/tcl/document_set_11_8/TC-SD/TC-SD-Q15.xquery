for $a in collection("11.8.4.dbxml")/dictionary/e
where contains ($a, "hockey")
return
    $a/hwg/hw
