#
#
# Usage: generateWix.sh <path_to_queryFile>
#
# Bash/shell script to generate a WiX .wxs file suitable for
# compilation using candle.exe
#
# Requirements:
#  python -- version 2.5 or higher needs to be in PATH
#  xqilla -- command line tool passed as $2 (uses the just-built version)
#

tmp_outfile="wixComponents.xml"
scriptDir=$1
XQILLA=$2
outfile=$scriptDir/dbxml_components.wxs
queryFile=$scriptDir/generateGroups.xq
envQueryFile=$scriptDir/generateEnv.xq
pyScript=$scriptDir/genWix.py
groupFile=wixGroups.xml
envFile=wixEnv.xml
compare=cmp

if [ ! -f $queryFile  -o ! -f $pyScript ]; then
    echo "Usage: generateWix.sh <path_to_winmsi>"
    exit 1
fi

echo "Generating $tmp_outfile"

if [ -f $outfile ]; then
    mv $outfile $outfile.save
fi

echo "python $pyScript $tmp_outfile"

python $pyScript $tmp_outfile

if [ ! -f $tmp_outfile ]; then
    echo "Failed to build $tmp_outfile"
    exit 1
fi

# XQilla expects file in same directory
mv $tmp_outfile $scriptDir/$tmp_outfile

echo "Adding ComponentGroup elements, group file is $groupFile queryfile is $queryFile"
$XQILLA -u -v "inFile" $tmp_outfile -v "groupFile" $groupFile $queryFile

echo "Adding Environment elements, environment file is $envFile queryfile is $envQueryFile"
$XQILLA -u -v "inFile" $tmp_outfile -v "envFile" $envFile $envQueryFile

# XQilla puts an XML decl in updated files, using version 1.1
# and the candle.exe WiX compiler doesn't handle 1.1, so
# until either of those can be changed ... hack
sed -e's!version="1.1"!version="1.0"!g' $scriptDir/$tmp_outfile > $outfile
rm $scriptDir/$tmp_outfile

echo "Generated $outfile"


