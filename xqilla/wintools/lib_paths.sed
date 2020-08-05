#	$Id: lib_paths.sed 812 2009-11-12 17:35:48Z jpcs $
# Default path for XQilla library, and paths to
# Xerces
#
#
# XQilla libraries are named:
#   xqilla<major_version><minor_version>{d}.{lib,dll,pdb}
# e.g.
#   xqilla22.dll (release) or xqilla22d.dll (debug)
#
# Assume dependent libraries are in ../../../XXX_HOME, e.g.
#   ../../../xerces-c-src
s!@XQILLA_STATIC_LIB@!@XQILLA_LIB@s!g
s!@XQILLA_LIB@!@XQILLA_LIB_BASE@@XQILLA_VERSION_MAJOR@@XQILLA_VERSION_MINOR@!g
s!@XQILLA_LIB_BASE@!xqilla!g

s!@XQILLA_VERSION_MAJOR@!2!g
s!@XQILLA_VERSION_MINOR@!2!g
s!@XQILLA_VERSION_PATCH@!3!g

# Xerces is assumed to be source (vs installation)
s!@XERCES_LIBHOME@!@XERCES_WINHOME@/Build/Win32/VC6!g
s!@XERCES_LIBHOME7@!@XERCES_WINHOME@/Build!g
s!@XERCES_LIBHOME8@!@XERCES_WINHOME@/Build!g
# Xerces-c is in ../../../xerces-c-src
s!@XERCES_WINHOME@!../../../@XERCES_NAME@!g
s!@XERCES_NAME@!xerces-c-src!g
s!@XERCES_STATIC_LIB@!xerces-c_static_@XERCES_VERSION_MAJOR@!g
s!@XERCES_LIB@!xerces-c_@XERCES_VERSION_MAJOR@!g
s!@XERCES_DLL@!xerces-c_@XERCES_VERSION_MAJOR@_@XERCES_VERSION_MINOR@!g

# Current Xerces version is 3.0.1
s!@XERCES_VERSION_MAJOR@!3!g
s!@XERCES_VERSION_MINOR@!0!g
s!@XERCES_VERSION_PATCH@!1!g

