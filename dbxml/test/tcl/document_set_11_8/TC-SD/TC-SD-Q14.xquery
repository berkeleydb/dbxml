for $a in collection("11.8.4.dbxml")/dictionary/e
where empty($a/vfl) and empty($a/et)
return
    <NoVFLnET>
        {$a/@id}
    </NoVFLnET>
