for $a in collection("11.8.2.dbxml")/catalog/item
where $a/date_of_release gt "1990-01-01" and
           $a/date_of_release lt "1991-01-01" and
           empty($a/publisher/contact_information/FAX_number)
return
      <Output>
        {$a/publisher/name}
      </Output>
