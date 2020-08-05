
use strict;
use warnings;

use Test::More tests => 28 ;
BEGIN { use_ok('Sleepycat::DbXml', 'simple') };
BEGIN { use_ok('File::Path') };


sub dumpException
{
    if (my $e = catch std::exception)
    {
        warn "caught exception ";
        warn $e->what() . "\n";
        exit( -1 );
    }
    elsif ($@)
    {
        warn "failed\n";
        warn $@;
        exit( -1 );
    }    
}

eval
{
    my $manager = new XmlManager() ;
    ok $manager, "created XmlManager ok";

    my $tmp_dir = "tmp" ;

    if (-d $tmp_dir)
    {
        rmtree($tmp_dir);
    }

    mkdir $tmp_dir, 0777 ;

    my $container_name = "$tmp_dir/fred1" ;
    my $context = $manager->createUpdateContext();
    
    my $c = $manager->createContainer($container_name); 
    
    my $doc = $manager->createDocument(); 
    ok defined $doc, "Document object created ok" ;
    $doc->setName("fred");
    $c->putDocument($doc);
    
    my $content = '<root><a a1="val1">a text</a><b><c cattr="c1"><d/></c></b></root>';
    $doc->setContent($content);

    my $cont = $doc->getContent();
    ok $cont eq $content, "getContent returned correct content" ;    

    my $reader = $doc->getContentAsEventReader();
    ok $reader, "created XmlEventReader ok";

    while ( $reader->hasNext() ){
        my $etype = $reader->next();
        #print "$etype\n" ;
#	puts "type [$reader getEventType]"
    }

    $reader->close();
    ok 1, "closed";

    ###################

    my $doc1 = $manager->createDocument(); 
    ok defined $doc1, "Document object created ok" ;
    $doc1->setName("joe");
    $c->putDocument($doc1);
    my $reader1 = $doc->getContentAsEventReader();
    ok $reader1, "created XmlEventReader ok";
    $doc1->setContentAsEventReader($reader1);

    #is $c->getDocument('joe')->getContent(), $content, "getContent returned correct content" ;    
    is $doc1->getContent(), $content, "getContent returned correct content" ;    
    #$reader1->close();
    ok 1, "closed";

    my $expected = '<root><a a1="val1">a text</a><b><c cattr="c1"><d/></c>b textplus</b></root>';

  {

    my $doc2 = $manager->createDocument(); 
    ok defined $doc2, "Document object created ok" ;
    $doc2->setName("jim");
    #$c->putDocument($doc2);
    my $writer = $c->putDocumentAsEventWriter($doc2, $context);
    ok $writer, "created XmlEventWriter ok";

    my $chars = XmlEventType::Characters;

    $writer->writeStartDocument();
    $writer->writeStartElement('root', undef, undef, 0, 0);
    $writer->writeStartElement('a', undef, undef, 1, 0);
    $writer->writeAttribute('a1', undef, undef, 'val1', 1);
    $writer->writeText($chars, 'a text');
    $writer->writeEndElement('a');
    $writer->writeStartElement('b', undef, undef, 0, 0);
    $writer->writeStartElement('c', undef, undef, 1, 0);
    $writer->writeAttribute('cattr', undef, undef, 'c1', 1);
    $writer->writeStartElement('d', undef, undef, 0, 1);
    $writer->writeEndElement('c');
    $writer->writeText($chars, 'b text');
    $writer->writeText($chars, 'plus');
    $writer->writeEndElement('b');
    $writer->writeEndElement('root');
    $writer->writeEndDocument();
    $writer->close();
    undef $writer;
    is $doc2->getContent(), $expected, "getContent returned correct content" ;    
    undef $doc2;
    undef $c;
  }  

    my $c1 = $manager->openContainer($container_name); 
    my $doc3 = $c1->getDocument('jim'); 
    is $doc3->getContent(), $expected, "getContent returned correct content" ;    
    ok 1, "closed";

    {

        my $doc31 = $manager->createDocument(); 
        ok defined $doc31, "Document object created ok" ;
        $doc31->setName("freddy");
        $c1->putDocument($doc31);
        $doc31->setContent($content);
        my $reader2 = $doc31->getContentAsEventReader();

        my $doc4 = $manager->createDocument(); 
        ok defined $doc4, "Document object created ok" ;
        $doc4->setName("harry");
        is $doc4->getContent(), '', "getContent returned correct content" ;    
        my $writer1 = $c1->putDocumentAsEventWriter($doc4, $context);

        my $rw = new XmlEventReaderToWriter($reader2, $writer1);
        $rw->start();

        is $doc4->getContent(), $content, "getContent returned correct content" ;    
    }

    {

        my $doc31 = $manager->createDocument(); 
        ok defined $doc31, "Document object created ok" ;
        $doc31->setName("freddy1");
        $c1->putDocument($doc31);
        $doc31->setContent($content);

        my $doc4 = $manager->createDocument(); 
        ok defined $doc4, "Document object created ok" ;
        $doc4->setName("harry1");
        is $doc4->getContent(), '', "getContent returned correct content" ;    
        my $writer1 = $c1->putDocumentAsEventWriter($doc4, $context);
        $doc31->getContentAsEventWriter($writer1);

        is $doc4->getContent(), $content, "getContent returned correct content" ;    
    }

    {

        my $doc31 = $manager->createDocument(); 
        ok defined $doc31, "Document object created ok" ;
        $doc31->setName("freddy2");
        $c1->putDocument($doc31);
        $doc31->setContent($content);
        my $reader = $doc31->getContentAsEventReader();

        $c1->putDocument('harry2', $reader);
        is $c1->getDocument('harry2')->getContent(), $content, "getContent returned correct content" ;    

    }

    {
        my $document = $manager->createDocument(); 
        $document->setName("bcbc");
        $document->setContent($content);
        my $docu =  new XmlValue($document);
        
        my $reader = $docu->asEventReader();
        ok $reader, "created XmlEventReader ok";

        $c1->putDocument('harry3', $reader);

        is $c1->getDocument('harry3')->getContent(), $content, "getContent returned correct content" ;    

    }
}
or dumpException();

