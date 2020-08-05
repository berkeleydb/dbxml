for $a in collection("11.8.3.dbxml")/article/prolog
where $a/dateline/country="Canada"
order by $a/dateline/date
return
    <Output>
        {$a/title}
        {$a/dateline/date}
    </Output>
