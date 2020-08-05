for $a in distinct-values
    (collection("11.8.4.dbxml")/dictionary/e/ss/s/qp/q[qd="1900"]/loc)
let $b := collection("11.8.4.dbxml")/dictionary/e/ss/s/qp/q[loc=$a]
return
    <Output>
        <Location>{$a}</Location>
        <NumberOfEntries>{count($b)}</NumberOfEntries>
    </Output>
