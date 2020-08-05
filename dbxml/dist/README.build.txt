
This is a README file for BDB XML developers adding and removing files
to the build as well as information about the build/release process
itself.  As a living document is needs to be kept updated and enhanced
as its users (developers) see fit.

It's somewhat task-oriented, describing how to do different common and
less common tasks in the tree.  Many of the tasks are best (or only)
done on Linux systems.

Overview of Build
-----------------

TBD -- describe Makefile.am, automake, relationship to windows build
files -- dbxml.template.xml, dbxml.projects, srcfiles.in


Adding a new file to the BDB XML library or other program
---------------------------------------------------------

Linux:

1.  Add the file to the appropriate location in Makefile.am
2.  Run automake in the top-level directory (Linux only)

Windows:
1.  Add the file to dist/dbxml.template.xml in the appropriate project
2.  Add the file to dist/srcfiles.in (see existing files for
template).  Be sure to maintain alphabetical order
3.  In dist, run "bash s_win32" to regenerate project files

Adding a new program or project to BDB XML
------------------------------------------

Linux:

1.  Look in Makefile.am for an example and copy it.  A recent
addition that may be useful as a template is the dbxml_unit_runner
2.  Add new files per instructions above
3.  Run automake in the top-level directory

Windows:

1.  Add the project to dist/dbxml.projects
2.  Add the project to dist/dbxml.template.xml -- this requires
using an existing example for cut/paste.  It may be necessary
to modify additional includes, preprocessor definitions, etc.
A new project requires its own GUID.  Generate one.  If you have
no handy guid-generating program, a simple way to do this on 
Linux machines is using Python:
    $ python
    >>> import uuid
    >>> uuid.uuid1()
This will generate a guid for you.
3.  Add new files per instructions above
4.  In dist run "bash s_win32" to regenerate projects









    







