for $a in collection("11.8.4.dbxml")/dictionary/e
where $a/hwg/hw="his"
return
    <Entry>
        {$a/ss}
    </Entry>
