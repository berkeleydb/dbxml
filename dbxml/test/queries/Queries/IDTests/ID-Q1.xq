<parttree>
{
	for $p in collection()/partlist/part[1 <= @id and @id <= 10] order by $p/@id
	return $p
}
</parttree>
