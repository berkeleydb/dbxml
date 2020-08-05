for $size in collection("11.8.2.dbxml")/catalog/item/attributes/size_of_book
where $size/length*$size/width*$size/height > 500000
return
    <Output>
        {$size/../../title}
    </Output>
