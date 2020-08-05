declare variable $collection external;

for $sale in collection($collection)/a
where $sale/@value[1][..]
return $sale