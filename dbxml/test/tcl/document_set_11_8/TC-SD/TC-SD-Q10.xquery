for $a in collection("11.8.4.dbxml")/dictionary/e
where $a/ss/s/qp/q/qd = "1900"
return
    <Output>
        {$a/hwg/hw}
        {$a/hwg/pr}
    </Output>
