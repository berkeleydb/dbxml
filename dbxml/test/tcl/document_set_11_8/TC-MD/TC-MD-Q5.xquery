for $a in collection("11.8.3.dbxml")/article[@id="9"]
return
    <HeadingOfSection>
        {$a/body/section[1]/@heading}
    </HeadingOfSection>
