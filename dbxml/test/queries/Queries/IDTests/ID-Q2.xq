<parttree>
{
	for $p in collection()/partlist/part[@id >= 1 and @id <= 10] order by $p/@id
	return $p
}
</parttree>
