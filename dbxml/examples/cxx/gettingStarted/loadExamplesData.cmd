@echo off
goto parseoption

:: usage loadExamplesData <path to dbenv> <path to example binaries>

:usage
echo.
echo You must compile exampleLoadContainer before running this script,
echo and you must add its directory to the PATH environment variable.
echo The PATH must also include paths to the Berkeley DB, Xerces and
echo Pathan DLLs.
echo.
echo When you run this script, You must identify the directory where you 
echo want to place your database environment, and you must identify the 
echo directory where these examples programs exist (use '.' if they are in
echo the current working directory).
echo.
echo The directory where you want to place your database
echo environment must also exist.
echo.
echo Use:
echo       loadExamplesData path_to_dbenv path_to_examples_binaries
echo.
pause
goto :EOF

:parseoption
set dbEnvDir=.\%1
set dataDir=%2\..\..\xmlData

if not exist %dbEnvDir% goto usage
if not exist %dataDir% goto usage

set loadCommand=dbxml_example_exampleLoadContainer

:: Windows bat files can't expand wildcards and there's a terrible short limit on the size of
:: a command line anyway, so we just loop through every file in the examples directory, loading
:: them one at a time.

echo "Loading container simpleExampleData.dbxml"
dir /b /a-d %dataDir%\simpleData\*.xml > loadExamplesData.tmp
%loadCommand% -h %dbEnvDir% -c simpleExampleData.dbxml -p %dataDir%\simpleData\ -f loadExamplesData.tmp

echo "Loading container namespaceExampleData.dbxml"
dir /b /a-d %dataDir%\nsData\*.xml > loadExamplesData.tmp
%loadCommand% -h %dbEnvDir% -c namespaceExampleData.dbxml -p %dataDir%\nsData\ -f loadExamplesData.tmp

