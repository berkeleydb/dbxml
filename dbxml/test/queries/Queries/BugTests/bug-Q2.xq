for $i in 0 to 4
return <d>{xs:dateTime('2008-12-29T00:00:00')+(xs:dayTimeDuration('P1D') * $i)}</d>
