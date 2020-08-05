for $a in collection("11.8.1.dbxml")/order[@id="3"]
return
    $a/*/order_line/item_id
