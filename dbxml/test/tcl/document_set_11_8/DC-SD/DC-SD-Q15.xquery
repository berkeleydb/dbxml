for $item in collection("11.8.2.dbxml")/catalog/item[@id="I7"],
    $related in collection("11.8.2.dbxml")/catalog/item
where $item/related_items/related_item/item_id = $related/@id
return
    <Output>
        {$related/title}
    </Output>
