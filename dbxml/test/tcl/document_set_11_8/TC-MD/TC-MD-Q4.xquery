for $a in collection("11.8.3.dbxml")/article[@id="8"]/body/section
    [@heading="introduction"],
    $p in collection("11.8.3.dbxml")/article[@id="8"]/body/section
    [. >> $a][1]
return
    <HeadingOfSection>
        {$p/@heading}
    </HeadingOfSection>
