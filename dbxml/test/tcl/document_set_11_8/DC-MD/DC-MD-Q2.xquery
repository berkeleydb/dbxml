for $a in distinct-values (collection("11.8.1.dbxml")/order
    [total > 11000.0]/customer_id)
let $b := collection("11.8.1.dbxml")/order[customer_id=$a]
return
    <Output>
        <CustKey>{$a}</CustKey>
        <NumberOfOrders>{count($b)}</NumberOfOrders>
    </Output>
