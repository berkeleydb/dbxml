#!/usr/bin/env python

import sys, os
refactor_path="/usr/local/lib/python3.2/"

def make2to3(path_from, path_to) :
    if os.path.isdir(path_from) :
        if path_from.endswith(".svn") : return
        try :
            os.mkdir(path_to)
        except :
            pass
        for i in os.listdir(path_from) :
            make2to3(path_from+"/"+i,path_to+"/"+i)
        return

    cwd = os.getcwd()
    if (not path_from.endswith(".py")) or (os.path.exists(path_to) and \
        (os.stat(path_from).st_mtime < os.stat(path_to).st_mtime)) :
            return
    print "*** Converting", path_to
    if path_from[0] != "/" :
        path_from = cwd+"/"+path_from
    if path_to[0] != "/" :
        path_to = cwd+"/"+path_to

    retcode = 0
    try :
        open(path_to, "w").write(open(path_from, "r").read())
        import subprocess, cStringIO
        process = subprocess.Popen(["2to3", "-w", path_to], cwd=refactor_path)
        retcode = process.wait()
    except :
        os.remove(path_to)
        raise

    try :
        os.remove(path_to+".bak")
    except :
        pass

    if retcode :
        os.remove(path_to)
        print "ERROR!"

    return bool(retcode)

print "Using '%s' for 2to3 conversion tool" %refactor_path

make2to3("setup2.py", "setup3.py")
make2to3("test2.py", "test3.py")
make2to3("Lib", "Lib3")
