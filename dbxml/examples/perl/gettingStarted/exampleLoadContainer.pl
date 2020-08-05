
use strict;
use warnings;

use Sleepycat::DbXml;
use IO::File ;

# some exception handling omitted for clarity

sub usage()
{
    warn <<EOM;
This program loads XML data into an identified container and environment.
Provide the directory where you want to place your database environment, the
name of the container to use, and the xml files you want inserted into the
container.

    -h <dbenv directory> -c <container> -f <filelist> -p <filepath> file1.xml file2.xml file3.xml .... 
EOM

        exit( -1 );
}

{
    # Used for metadata example
    my $mdURI = "http://dbxmlExamples/timestamp";
    my $mdName = "timeStamp";

    my $path2DbEnv;
    my $theContainer;
    my $filePath;
    my @files2add;

    while (@ARGV && $ARGV[0] =~ /^-/ )
    {
        my $opt = shift @ARGV ;

        if ($opt =~ s/^-h//)
          { $path2DbEnv = $opt || shift }
        elsif ($opt =~ s/^-c//)
          { $theContainer = $opt || shift }
        elsif ($opt =~ s/^-p//)
          { $filePath = $opt || shift }
        elsif ($opt =~ s/^-f//) {
            my $file = $opt || shift ;
            open F, "<$file" || die "Cannot open '$file': $!\n" ;
            while (<F>)
            {
                chomp;
                push @files2add, $_ ;
            }
        }
        else
          { usage() }
    }

    push @files2add, @ARGV ;

    # You must provide a container name, a path to a database environment,
    # and at least one file to add to the container.
    if ( ! $path2DbEnv || ! $theContainer || ! @files2add )
    {
        usage();
    }

    # Open a container in the db environment
    my $env = new DbEnv(0);
    $env->set_cachesize(0, 64 * 1024 * 1024, 1);
    $env->open($path2DbEnv,
                Db::DB_INIT_MPOOL|Db::DB_CREATE|Db::DB_INIT_LOCK|Db::DB_INIT_LOG|Db::DB_INIT_TXN, 0);
    my $theMgr = new XmlManager($env);

    my $containerTxn = $theMgr->createTransaction();
    my $xcc = new XmlContainerConfig();
    $xcc->setAllowCreate(1);
    my $container = $theMgr->openContainer($containerTxn, $theContainer, $xcc);
    $containerTxn->commit();

    # myDbEnv and myXmlContainer open with transactions. All subsequent
    # writes to them must also be performed inside a transaction.

    # Iterate through the list of files to add, read each into a string and 
    #   put that string into the now opened container.
    eval {
      # Get a transaction
      my $txn = $theMgr->createTransaction();

      foreach my $theFile (@files2add)
      {
          #  Load the xml document into a string 
          my $xmlString;
          my $inFile = new IO::File "<$theFile"
                        or die "Cannot open $theFile: $!\n" ;
          while (<$inFile>)
          {
              $xmlString .= $_ ;
          }          

          $inFile->close();

          #  declare an xml document 
          my $xmlDoc = $theMgr->createDocument();

          #  Set the xml document's content to be the xmlString we just obtained. 
          $xmlDoc->setContent( $xmlString );

          #  Get the document name. this strips off any path information.
          my $theName = $theFile;
          $theName =~ s#.*/## ;

          # Set the document name
          $xmlDoc->setName( $theName );

          # Set sample metadata who's value is the current date and time.
          # the variables used here are defined in gettingStarted.hpp

          # Get the local time
          my $timeString = time ;

          # Set the localtime onto the timeStamp metadata attribute
          $xmlDoc->setMetaData( $mdURI, $mdName, $timeString );

          #  place that document into the container 
          $container->putDocument($txn, $xmlDoc);

          print "\tAdded $theFile to container $theContainer.\n" ;

      } # end files2add iterator

      $txn->commit();
    } ;

    if (my $e = catch std::exception)
    {
        warn "Error adding XML data to container $theContainer\n" ;
        warn $e->what() . "\n";
        exit( -1 );
    }
    elsif ($@)
    {
        warn "Error adding XML data to container $theContainer\n" ;
        warn $@;
        exit( -1 );
    }

    exit 0 ;
}
