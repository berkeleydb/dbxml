for $a in collection("11.8.2.dbxml")/catalog/item
where contains ($a/description, "hockey")
return
<Output>
    {$a/@id}
</Output>
