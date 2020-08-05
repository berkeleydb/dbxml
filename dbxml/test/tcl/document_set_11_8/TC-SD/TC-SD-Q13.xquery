for $a in collection("11.8.4.dbxml")/dictionary/e
where $a/hwg/hw="his"
return
    <Output>
        {$a/hwg/hw}
        {$a/hwg/pr}
        {$a/hwg/pos}
        {$a/etymology/cr[1]}
        {$a/ss/s[1]/def}
    </Output>
