# -*- coding: utf-8 -*-
#
# Python documentation build configuration file
#
# This file is execfile()d with the current directory set to its containing dir.
#
# The contents of this file are pickled, so don't put values in the namespace
# that aren't pickleable (module imports are okay, they're removed automatically).

import sys, os, time
sys.path.append('tools/sphinxext')

# General configuration
# ---------------------

#extensions = ['sphinx.ext.refcounting', 'sphinx.ext.coverage']

# General substitutions.
project = 'PyBSDDB'
copyright = '2008-2009 Jesus Cea Avion'

# The default replacements for |version| and |release|.
#
# The short X.Y version.
version = '4.8.1'
# The full version, including alpha/beta/rc tags.
release = '4.8.1'

# There are two options for replacing |today|: either, you set today to some
# non-false value, then it is used:
today = ''
# Else, today_fmt is used as the format for a strftime call.
today_fmt = '%B %d, %Y'

# List of files that shouldn't be included in the build.
unused_docs = [
    'whatsnew/2.0',
    'whatsnew/2.1',
    'whatsnew/2.2',
    'whatsnew/2.3',
    'whatsnew/2.4',
    'whatsnew/2.5',
    'maclib/scrap',
    'library/xmllib',
    'library/xml.etree',
]

# Relative filename of the reference count data file.
#refcount_file = 'data/refcounts.dat'

# If true, '()' will be appended to :func: etc. cross-reference text.
add_function_parentheses = True

# If true, the current module name will be prepended to all description
# unit titles (such as .. function::).
add_module_names = True


# Options for HTML output
# -----------------------

# If not '', a 'Last updated on:' timestamp is inserted at every page bottom,
# using the given strftime format.
html_last_updated_fmt = '%b %d, %Y'

# If true, SmartyPants will be used to convert quotes and dashes to
# typographically correct entities.
html_use_smartypants = True

# Content template for the index page, filename relative to this file.
#html_index = 'tools/sphinxext/indexcontent.html'

# Custom sidebar templates, filenames relative to this file.
#html_sidebars = {
#    'index': 'tools/sphinxext/indexsidebar.html',
#}

# Additional templates that should be rendered to pages.
#html_additional_pages = {
#    'download': 'tools/sphinxext/download.html',
#}

# Output file base name for HTML help builder.
#htmlhelp_basename = 'python' + release.replace('.', '')

html_copy_source=False

