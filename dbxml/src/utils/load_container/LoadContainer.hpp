//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef LOADCONTAINER_HPP
#define LOADCONTAINER_HPP

#include <string>
#include <vector>

namespace DbXml {
	class XmlManager;
}

/**
 * A class to add documents to a specified container
 *
 */
class LoadContainer {

public:
	LoadContainer(DbXml::XmlManager &mgr,
		      const std::string &containerName, 
		      bool verbose);
	~LoadContainer() {}
	
	int addFiles(const std::vector<std::string > &fileList, 
		const std::string &pathPrepend, bool failOnError) const;
	void addFile(const std::string &fileName) const;
	int addFileList(const std::string &pathPrepend,
		const std::string &fileName, bool failOnError) const;

	int count() const { return _count; }
private:
 
	std::string getNameFromPath(const std::string &theFile) const;

	DbXml::XmlManager &_mgr;
	std::string _conName;
	bool _verbose;
	mutable int _count;
};
#endif
