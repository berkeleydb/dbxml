for $a in collection("11.8.1.dbxml")/order[@id="5"]
return
    <Output>
        {$a/order_lines}
    </Output>
