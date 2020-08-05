for $a in collection("11.8.1.dbxml")/order
where empty($a/order_lines/order_line[2])
return
    <OneItemLine>
        {$a/@id}
    </OneItemLine>
