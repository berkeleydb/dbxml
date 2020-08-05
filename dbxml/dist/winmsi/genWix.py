#
#
# genWix.py is used to generate a WiX .wxs format file that
# can be compiled by the candle.exe WiX compiler.
#
# Usage: python genWix.py <output_file>
#
# The current directory is expected to be dbxml-2.4.x in a tree
# that has been fully built for Windows and includes:
#   o release (debug no longer created)
#   o java, perl, python, and php binaries
#   o the normal source tree
# 
# The list of directories traversed is at the bottom of this script,
# in "main" and includes:
#   bin,lib,include,jar,perl,python,php,dbxml
#
# "Excluded" directories/files are listed below in the GenWix class
# constructor.  These will *not* be included in the packaging.
#
# The output file is expected to be post-processed using XQuery Update
# to add ComponentGroup elements for the various WiX Feature elements.
# 

import sys
import os

class GenWix:
    def __init__(self, sourcePfx, outfile, dbg):
	self.debugOn = dbg
	self.componentId = 0
	self.indentLevel = 0
	self.indentIncr = 2
        self.shortId = 0
	self.fragName="all"
	self.refDirectory = "INSTALLDIR"
	self.compPrefix = "comp"
	self.dirPrefix = "dir"
	self.sourcePrefix = os.path.normpath(sourcePfx)
	self.excludes = [os.path.normpath("dbxml/build_windows/Win32"),
			 os.path.normpath("dbxml/build_windows/db.jar"),
			 os.path.normpath("dbxml/build_windows/dbxml.jar"),
			 os.path.normpath("dbxml/build_windows/dbxmlexamples.jar"),
			 os.path.normpath("dbxml/build_unix"),
			 os.path.normpath("dbxml/include"),
			 os.path.normpath("dbxml/dist"),
			 os.path.normpath("dbxml/src"),
			 os.path.normpath("dbxml/test"),
			 os.path.normpath("CVS")]
	self.outputFile = outfile
	self.out = open(self.outputFile, "ab")
	self.out.truncate(0)

    def __del__(self):
	self.out.close()

    def checkExclude(self, fname):
	for ex in self.excludes:
	    if fname.find(ex) != -1:
		return True
        return False

    # NOTE: this will count leading/trailing '/'
    def count(self, path):
	return len(path.split("/"))

    def nextId(self):
	self.componentId = self.componentId + 1

    def printComponentId(self, fragname):
	return self.makeId("%s_%s_%d"%(self.compPrefix,fragname,self.componentId))

    def printDirectoryId(self,dirname):
	return self.makeId("%s_%s_%d"%(self.dirPrefix,dirname,self.componentId))

    def indent(self, arg):
	if arg == "-" and self.indentLevel != 0:
	    self.indentLevel = self.indentLevel - self.indentIncr
	i = 0
	while i != self.indentLevel:
	    self.out.write(" ")
	    i = i+1
	if arg == "+":
	    self.indentLevel = self.indentLevel + self.indentIncr

    def echo(self, arg, indentArg):
	self.indent(indentArg)
	#sys.stdout.write(arg+"\n")
	self.out.write(arg+"\n")

    def generateGuid(self):
	if sys.version_info[1] < 5:
	    return "REPLACE_WITH_GUID"
	else:
	    import uuid
	    return uuid.uuid1()

    # used by makeShortName                                                            
    def cleanName(self, name):
	for c in ("-","%","@","!"):
	    name = name.replace(c,"")
	return name

    def makeId(self, id):
        return id.replace("-","_")

    # turn names into Windows 8.3 names.
    # A semi-unique "ID" is inserted, using 3 bytes of hex,
    # which gives us a total of 4096 "unique" IDs.  If
    # that number is exceeded in one class instance, a bad
    # name is returned, which will eventually cause a 
    # recognizable failure.  Names look like: ABCD~NNN.EXT
    # E.g. NAMEISLONG.EXTLONG => NAME~123.EXT
    #
    def makeShortName(self, longName):
	name = longName.upper()
        try:
            index = name.find(".")
        except ValueError:
            index = -1
        
        if index == -1:
            if len(name) <= 8:
                return longName
            after = ""
        else:
	    if index <= 8 and (len(name) - index) <= 4:
	        return longName
            after = "." + name[index+1:index+4]
            after = self.cleanName(after)
            
	self.shortId = self.shortId + 1
	if self.shortId >= 4096:   # check for overflow of ID space
	    return "too_many_ids.bad" # will cause a failure...
	hid = hex(self.shortId)
	name = self.cleanName(name) # remove stray chars
	# first 5 chars + ~ + Id + . + extension
	return name[0:4]+"~"+str(hid)[2:5]+after

    def makeFullPath(self, fname, root):
	return os.path.join(self.sourcePrefix,os.path.join(root,fname))

    def makeNames(self, fname):
        shortName = self.makeShortName(fname)
        if shortName != fname:
            longName="LongName=\'%s\'"%fname
        else:
            longName=""
        return "Name=\'%s\' %s"%(shortName,longName)

    def generateFile(self, fname, root, dirId):
	# allow exclusion of individual files
	if self.checkExclude(os.path.join(root,fname)):
	    self.debug("excluding %s\n"%os.path.join(root,fname))
	    return
        idname = self.makeId("%s_%s"%(dirId,fname))
	elem ="<File Id=\'%s\' Compressed='yes' %s Source=\'%s\' />"%(idname,self.makeNames(fname),self.makeFullPath(fname, root))
	self.echo(elem,"")
	
    def startDirectory(self, dir, parent):
	# use parent dirname as part of name for more uniqueness
	self.debug("Starting dir %s"%dir)
	self.nextId()
	#if (len(parent) != 0):
#            dirname = "%s_%s"%(parent,dir)
#	else:
#	    dirname = dir
        dirId = self.printDirectoryId(dir)
        elem ="<Directory Id=\'%s\' %s>"%(dirId,self.makeNames(dir))
	celem ="<Component Id=\'%s\' DiskId='1' KeyPath='yes' Guid=\'%s\'>"%(self.printComponentId(self.fragName),self.generateGuid())
	self.echo(elem,"+")
	self.echo(celem,"+")
        return dirId
	
    def endDirectory(self, dir):
	self.debug("Ending dir %s"%dir)
	self.echo("</Directory>","-")

    def endDirectoryComponent(self, dir):
	self.debug("Ending Component for dir %s"%dir)
	self.echo("</Component>","-")

    def generatePreamble(self):
	# leave off the XML decl and Wix default namespace -- candle.exe
	# doesn't seem to care and it makes updating simpler
	self.echo("<Wix>","+")
	self.echo("<Fragment>","+")
	self.echo("<DirectoryRef Id='%s'>"%self.refDirectory,"+")

    def generateClose(self):
	self.echo("</DirectoryRef>","-")
	self.echo("</Fragment>","-")
	self.echo("</Wix>","-")

    def debug(self, msg):
	if self.debugOn:
	    sys.stdout.write(msg+"\n")

    def generateDir(self, dir, path):
	fullPath = os.path.join(path,dir)
	if self.checkExclude(fullPath):
	    self.debug("excluding %s\n"%fullPath)
	    return
	# ignore top-level directories that are missing, or other
	# errors (e.g. regular file)
	try:
	    files = os.listdir(fullPath)
	except:
	    return
	# check for empty dir (this won't detect directories that contain
	# only empty directories -- just don't do that...)
	if len(files) == 0:
	    self.debug("skipping empty dir %s"%dir)
	    return

	dirId = self.startDirectory(dir, os.path.basename(path))

	# process regular files before directories
	fileList = [f for f in files if os.path.isfile(os.path.join(fullPath,f))]
	for file in fileList:
		self.generateFile(file,fullPath, dirId)

        # Component element must end before subdirectories start
	self.endDirectoryComponent(dir)
	# now directories
	dirList = [d for d in files if os.path.isdir(os.path.join(fullPath,d))]
	for directory in dirList:
		self.generateDir(directory, fullPath)
	self.endDirectory(dir)

    def generateRequiredFiles(self):
        # LICENSE.txt, README.txt
       	celem ="<Component Id='license_readme' DiskId='1' KeyPath='yes' Guid=\'%s\'>"%self.generateGuid()
	self.echo(celem,"+")
	elem ="<File Id='LICENSE.txt' Name='LICENSE.txt' Source=\'%s\' />"%self.makeFullPath("dbxml\\LICENSE", "")
	self.echo(elem,"")
	elem ="<File Id='README.txt' Name='README.txt' Source=\'%s\' />"%self.makeFullPath("dbxml\\README", "")
	self.echo(elem,"")
        self.echo("</Component>","-")
           
    def generate(self, directories):
	self.generatePreamble()
        self.generateRequiredFiles()
	for dir in directories:
	    self.generateDir(dir, "")
	self.generateClose()
#
# Main script
#
if __name__ == "__main__":
    outfile = sys.argv[-1]
    if outfile == sys.argv[0]:
	print "Usage: genWix.py <output_file>"
	sys.exit()

    print "Generating into file: " + outfile
    gw = GenWix(os.path.realpath("."),outfile,False)
    gw.generate(["bin","lib","include","jar","perl","python","php","dbxml"])   
