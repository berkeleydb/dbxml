for $word in collection("11.8.4.dbxml")/dictionary/e
where every $item in $word/ss/s/qp/q
    satisfies $item/qd eq "1900"
return
    $word
