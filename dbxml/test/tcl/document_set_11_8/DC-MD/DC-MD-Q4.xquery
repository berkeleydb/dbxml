for $a in collection("11.8.1.dbxml")/order[@id="2"]
return
    $a/order_lines/order_line[1]
