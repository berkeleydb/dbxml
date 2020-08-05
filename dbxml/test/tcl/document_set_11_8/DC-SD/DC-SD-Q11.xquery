for $a in collection("11.8.2.dbxml")/catalog/item
where $a/date_of_release gt "1990-01-01" and
    $a/date_of_release lt "1995-01-01"
order by $a/date_of_release descending
return
    <Output>
        {$a/title}
        {$a/date_of_release}
    </Output>
