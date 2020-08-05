for $a in collection("11.8.3.dbxml")/article/prolog
where empty ($a/genre)
return
    <NoGenre>
        {$a/title}
    </NoGenre>
