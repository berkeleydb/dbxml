/*
  Takes the full path to a container and prints out what databases it contains
  and the makeup of each database on disk.

  Build on Linux/Unix using "make dbxml_disk" in dbxml/build_unix.

  Have to change USE_DBXML_DISK from 0 to 1 in Container.hpp to make this work properly.
 */

#include <string>
#include <dbxml/DbXml.hpp>
#include "../../src/dbxml/Container.hpp"

using namespace DbXml;

/*
 * usage --
 *	Display the usage message.
 */
int
usage()
{
	(void)fprintf(stderr, "%s\n",
		      "usage: dbxml_disk full_container_path");
	return -1;
}


int main(int argc, char *argv[]) {
	if(argc != 2)
		return usage();
	std::string containerName(argv[1]);
	
	try {
		XmlManager mgr;
		XmlContainer cont = mgr.openContainer(containerName);
		XmlTransaction txn;
		std::string dbInfo = (*cont).disk(txn);
		std::cout << dbInfo << std::endl;
	}
	catch (XmlException& e){
		std::cout << e.what() << std::endl;
		return -1;
	}
	return 0;
}
