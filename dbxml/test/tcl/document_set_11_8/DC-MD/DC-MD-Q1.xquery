for $order in collection("11.8.1.dbxml")/order[@id="1"]
return
    $order/customer_id
