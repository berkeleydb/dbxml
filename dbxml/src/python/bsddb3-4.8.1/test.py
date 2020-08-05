#!/usr/bin/env python

import sys
if sys.version_info[0] == 2 :
  import test2 as test
else :  # >= Python 3.0
  import test3 as test

sys.path.append("/home/pybsddb/trunk/build/lib.solaris-2.10-i86pc-3.1/bsddb3/")

if __name__ == "__main__":
    test.process_args()

