declare namespace tnt="http://tntbase.mathweb.org/ns";
declare default element namespace "http://omdoc.org/ns";

declare function local:direct_imp($theory_name as xs:string) as node()* {

    collection()[dbxml:metadata('tnt:revnum')=-1]//theory[ends-with(./imports/@from, $theory_name)]/@xml:id

};

declare function local:imp($theory_name as xs:string) as node()* {

    let $direct := local:direct_imp($theory_name)
    return for $i in $direct return ($i | local:imp($i))

};

local:imp('sets-introduction')
