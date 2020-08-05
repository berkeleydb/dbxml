for $ent in collection("11.8.4.dbxml")/dictionary/e[@id="E1"],
    $related in collection("11.8.4.dbxml")/dictionary/e
where $ent/et/cr = $related/@id
return
    <Output>
        {$related/hwg/hw}
    </Output>
