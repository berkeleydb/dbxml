(: 
*
* Usage: xqilla -u -v "inFile" <input_file> -v "envFile" <group_file> <this_file_name>
*
* XQilla will update the file ($inFile) in-place, so it should
* use a copy if the original is to be saved.
*
* This XQuery script creates WiX Environment elements and inserts them
* into the $inFile as directed by content in $envFile.
*
:)

(: preamble -- shut up revaliation and declare input files :)
declare revalidation skip;
declare variable $inFile as xs:untypedAtomic external;
declare variable $envFile as xs:untypedAtomic external;

declare function local:getEnv()
{
	doc($envFile)/envVars/env
};

(: this function is complicated by the need to uniquely identify
   Directory elements in the tree, e.g. "debug" :)
declare function local:getComponent($env)
{
    let $dir := $env/@name
    let $d := if (not(empty($env/@parent))) then
                 if (not(empty($env/@parentName))) then
                    doc($inFile)//Directory[@Name=$dir or @LongName=$dir][name(..)=$env/@parent][../@Name=$env/@parentName]
                 else
                    doc($inFile)//Directory[@Name=$dir or @LongName=$dir][name(..)=$env/@parent]
              else
                 doc($inFile)//Directory[@Name=$dir or @LongName=$dir]
	  return $d/Component
};

declare updating function local:createEnv($env)
{
	let $comp := local:getComponent($env)
	return if (not(empty($comp))) then
           for $evar in $env/Environment
              return insert node $evar as first into $comp
	else ()
};

(: the main query :)
for $env in local:getEnv()
        return local:createEnv($env)


