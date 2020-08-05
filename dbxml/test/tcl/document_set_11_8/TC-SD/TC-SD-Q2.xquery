for $ent in collection("11.8.4.dbxml")/dictionary/e
where $ent/ss/s/qp/q/qd="1900"
return
    $ent/hwg/hw
