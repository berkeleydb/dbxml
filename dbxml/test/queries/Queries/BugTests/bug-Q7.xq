for $doc1 in collection()
where 
   ($doc1/root/a = '1' and $doc1/root/b = '1') or 
   $doc1/root/a = '2'
return $doc1
