let $ent := collection("11.8.4.dbxml")/dictionary/e[hwg/hw="you"]
for $prevEnt in collection("11.8.4.dbxml")/dictionary/e[some $a in hwg/hw satisfies $a << $ent]
    [position() = last()]
return
    <Output>
        <CurrentEntry>{$ent/hwg/hw/text()}</CurrentEntry>
        <PreviousEntry>{$prevEnt/hwg/hw/text()}</PreviousEntry>
    </Output>
