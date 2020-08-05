#!/bin/sh
#
# See the file LICENSE for redistribution information.
#
# Copyright (c) 2004,2009 Oracle.  All rights reserved.
#
#

path2dbEnv=$1
path2examples=$2

# usage: loadExamplesData.sh <path to dbenv> <path to examples>
if [ ! -d "$path2dbEnv" ]
then
	echo "Error: You must provide a path to the directory where you want to place"
	echo "the database environment, and this directory must currently exist." 
	echo "Use:"
	echo "loadExamplesData.sh <path to dbenv> <path to example executables>"
	exit
fi

if [ ! -d "$path2examples" ]
then
	echo "Error: You must provide the path to the directory where your C++"
	echo "  gettingStarted example executables are placed." 
	echo "Use:  loadExamplesData.sh <path to dbenv> <path to examples>"
	echo "path2data: $path2examples"
	exit
fi

loadCommand="$path2examples/exampleLoadContainer"
echo $loadCommand
if [ ! -f $loadCommand ]
then
	echo "Error: $loadCommand does not exist. You must compile this application before"
	echo "  running this script."
	exit
fi

simpleData=`ls ../../xmlData/simpleData/*.xml`
namespaceData=`ls ../../xmlData/nsData/*.xml`

echo "Loading container simpleExampleData"
$loadCommand -h $path2dbEnv -c simpleExampleData.dbxml $simpleData
echo "Loading container namespaceExampleData"
$loadCommand -h $path2dbEnv -c namespaceExampleData.dbxml $namespaceData
