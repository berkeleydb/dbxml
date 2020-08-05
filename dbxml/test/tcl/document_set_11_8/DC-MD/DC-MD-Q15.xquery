for $order in collection("11.8.1.dbxml")/order,
    $cust in collection("11.8.1.dbxml")/customers/customer
where $order/customer_id = $cust/@id
    and $order/@id = "7"
return
    <Output>
        {$order/@id}
        {$order/order_status}
        {$cust/first_name}
        {$cust/last_name}
        {$cust/phone_number}
    </Output>
