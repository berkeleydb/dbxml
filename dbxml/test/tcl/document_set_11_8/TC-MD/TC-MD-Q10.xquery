for $a in collection("11.8.3.dbxml")/article/prolog
order by $a/dateline/country
return
    <Output>
        {$a/title}
        {$a/dateline/country}
    </Output>
