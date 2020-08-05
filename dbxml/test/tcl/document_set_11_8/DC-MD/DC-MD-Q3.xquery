let $item := collection("11.8.1.dbxml")/items/item[@id="8"]
for $prevItem in collection("11.8.1.dbxml")/items/item
    [. << $item][position() = last()]
return
    <Output>
        <CurrentItem>{$item/@id}</CurrentItem>
        <PreviousItem>{$prevItem/@id}</PreviousItem>
    </Output>
