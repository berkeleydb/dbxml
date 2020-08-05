let $item := collection("11.8.2.dbxml")/catalog/item[@id="I2"]
for  $prevItem in collection("11.8.2.dbxml")/catalog/item
    [. << $item][position() = last()]
return
    <Output>
        <CurrentItem>{$item/@id}</CurrentItem>
        <PreviousItem>{$prevItem/@id}</PreviousItem>
    </Output>
