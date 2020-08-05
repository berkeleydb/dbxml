for $a in collection("11.8.4.dbxml")/dictionary/e
    [hwg/hw="the"]/ss/s/qp/q
order by $a/qd
return
    <Output>
        {$a/a}
        {$a/qd}
    </Output>
