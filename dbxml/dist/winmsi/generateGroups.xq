(: 
*
* Usage: xqilla -u -v "inFile" <input_file> -v "groupFile" <group_file> <this_file_name>
*
* XQilla will update the file ($inFile) in-place, so it should
* use a copy if the original is to be saved.
*
* This XQuery script creates WiX ComponentGroup elements inside a
* generated XML file ($inFile). The groups are defined by another 
* file ($groupFile) and specify a directory, or set of directories 
* (optionally recursive) to add to a specific group. The groups specified
* include:
*
* group_bin -- release runtime (bin, without debug)
* group_debug -- debug runtime (bin/debug, without bin) : NOTE: no longer included
* group_doc -- documentation (dbxml/doc -- recursive)
* group_devo -- development files (include, lib -- recursive)
* group_examples -- examples in all languages (dbxml/examples -- recursive,
*      dbxml/build_windows (empty))
* group_java -- java runtime (jar -- recursive)
* group_python -- python runtime (python -- recursive)
* group_perl -- perl runtime (perl -- recursive)
* group_php -- php runtime (php -- recursive)
:)

(: preamble -- shut up revaliation and declare input files :)
declare revalidation skip;
declare variable $inFile as xs:untypedAtomic external;
declare variable $groupFile as xs:untypedAtomic external;

declare function local:getFrag()
{
	doc($inFile)/Wix/Fragment
};

(: this function is complicated by the need to uniquely identify
   Directory elements in the tree, e.g. "debug" and "dbxml" :)
declare function local:getComponents($group)
{
	for $dir in $group/dir
          let $d := if (not(empty($dir/@parent))) then
                        if (not(empty($dir/@parentName))) then
                            doc($inFile)//Directory[@Name=$dir or @LongName=$dir][name(..)=$dir/@parent][../@Name=$dir/@parentName]
                        else
                            doc($inFile)//Directory[@Name=$dir or @LongName=$dir][name(..)=$dir/@parent]
                    else
                        doc($inFile)//Directory[@Name=$dir or @LongName=$dir]
	  return 
	      if ($dir/@recurse="yes") then 
		 $d//Component
	      else
		 $d/Component
};

declare function local:getGroups()
{
	doc($groupFile)/groups/group
};

declare function local:createGroup($group)
{
	let $comp := local:getComponents($group)
	return if (not(empty($comp))) then
	       <ComponentGroup Id="{concat("group_",$group/@name)}">
	   	  {for $c in $comp return
	           <ComponentRef Id="{$c/@Id}"/>}
	       </ComponentGroup>
	else ()
};

(: the main query :)
for $group in local:getGroups()
        return insert node local:createGroup($group) as
	first into local:getFrag()
