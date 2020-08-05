for $a in collection("11.8.1.dbxml")/authors/author
where contains ($a/biography, "hockey")
return
<Output>
    {$a/@id}
</Output>
