for $a in collection("11.8.3.dbxml")/article[@id='17']/epilog/references/a_id,
    $b in collection("11.8.3.dbxml")/article
where $a = $b/@id
return
    <Output>
        {$b/prolog/title}
    </Output>
