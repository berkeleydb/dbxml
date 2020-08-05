for $a in distinct-values (collection("11.8.2.dbxml")/catalog/item
    [date_of_release >= "1990-01-01"]
    [date_of_release < "1991-01-01"]/publisher/name)
let $b := collection("11.8.2.dbxml")/catalog/item/publisher[name=$a]
return
    <Output>
        <Publisher>{$a}</Publisher>
        <NumberOfItems>{count($b)}</NumberOfItems>
    </Output>
