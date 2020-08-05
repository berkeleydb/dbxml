.. $Id: history.rst 393 2008-03-26 00:03:51Z jcea $

History
-------

.. _Digital Creations: http://www.digicool.com/
.. _wxPython: http://www.wxpython.org/

This module was started by Andrew Kuchling (amk) to remove the
dependency on SWIG in a package by Gregory P. Smith who based his work
on a similar package by Robin Dunn which wrapped Berkeley DB 2.7.x.

Development then returned full circle back to Robin Dunn working in
behalf of `Digital Creations`_ to complete the SWIG-less wrapping of the DB
3.x API and to build a solid unit test suite. Having completed that,
Robin is now busy with another project (wxPython_) and Greg has returned
as maintainer.

Jesus Cea Avion is the maintainer of this code since February 2008.

This module is included in the standard python >= 2.3 distribution as
the bsddb module. The only reason you should look here is for
documentation or to get a more up to date version. The bsddb.db module
aims to mirror much of the Berkeley DB C/C++ API. 

