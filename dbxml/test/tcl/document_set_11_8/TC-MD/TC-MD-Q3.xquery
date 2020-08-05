for $a in distinct-values (collection("11.8.3.dbxml")/article/prolog/dateline/date)
let $b := collection("11.8.3.dbxml")/article/prolog/dateline[date=$a]
return
    <Output>
        <Date>{$a}</Date>
        <NumberOfArticles>{count($b)}</NumberOfArticles>
    </Output>
