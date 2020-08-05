for $a in collection("11.8.3.dbxml")/article[@id="4"]
return
    <Article>
        {$a/body}
    </Article>
