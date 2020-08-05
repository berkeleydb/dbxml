for $a in collection("11.8.1.dbxml")/order
where $a/total > 11000.0
order by $a/total descending
return
    <Output>
        {$a/@id}
        {$a/order_date}
        {$a/total}
    </Output>
