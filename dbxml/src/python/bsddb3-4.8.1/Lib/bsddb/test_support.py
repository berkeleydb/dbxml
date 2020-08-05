# This module is a bridge.
#
# Code is copied from Python 2.6 (trunk) Lib/test/test_support.py that
# the bsddb test suite needs even when run standalone on a python
# version that may not have all of these.

# DO NOT ADD NEW UNIQUE CODE.  Copy code from the python trunk
# trunk test_support module into here.  If you need a place for your
# own stuff specific to bsddb tests, make a bsddb.test.foo module.

import errno
import os
import shutil
import socket

def unlink(filename):
    try:
        os.unlink(filename)
    except OSError:
        pass

def rmtree(path):
    try:
        shutil.rmtree(path)
    except OSError, e:
        # Unix returns ENOENT, Windows returns ESRCH.
        if e.errno not in (errno.ENOENT, errno.ESRCH):
            raise

def find_unused_port(family=socket.AF_INET, socktype=socket.SOCK_STREAM):
    tempsock = socket.socket(family, socktype)
    port = bind_port(tempsock, family=family, socktype=socktype)
    tempsock.close()
    del tempsock
    return port

HOST = 'localhost'
def bind_port(sock, family, socktype, host=HOST):
    if family == socket.AF_INET and type == socket.SOCK_STREAM:
        if hasattr(socket, 'SO_REUSEADDR'):
            if sock.getsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR) == 1:
                raise TestFailed("tests should never set the SO_REUSEADDR "   \
                                 "socket option on TCP/IP sockets!")
        if hasattr(socket, 'SO_REUSEPORT'):
            if sock.getsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT) == 1:
                raise TestFailed("tests should never set the SO_REUSEPORT "   \
                                 "socket option on TCP/IP sockets!")
        if hasattr(socket, 'SO_EXCLUSIVEADDRUSE'):
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_EXCLUSIVEADDRUSE, 1)

    sock.bind((host, 0))
    port = sock.getsockname()[1]
    return port

