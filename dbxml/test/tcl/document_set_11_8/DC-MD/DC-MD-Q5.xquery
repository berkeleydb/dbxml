for $ord in collection("11.8.1.dbxml")/order
where some $item in $ord/order_lines/order_line
    satisfies $item/discount_rate > 0.02
return
    $ord
