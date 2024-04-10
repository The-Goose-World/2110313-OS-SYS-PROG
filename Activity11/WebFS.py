#!/usr/bin/env python3

# 2110352 Operating System
# FUSE/Filesystem exercise
# By Krerk Piromsopa, Ph.D. <Krerk.P@chula.ac.th>
#    Department of Computer Engineering
#    Chulalongkorn University.

import os, stat, errno
import fuse
from fuse import Fuse
import requests

if not hasattr(fuse, '__version__'):
    raise RuntimeError("your fuse-py doesn't know of fuse.__version__, probably it's too old.")
       
fuse.fuse_python_api = (0, 2)

particapation_path = "/participation"

def myRead():
  req = requests.get("https://mis.cp.eng.chula.ac.th/krerk/teaching/2022s2-os/status.php");
  content = bytes(req.text.encode("utf-8"))
  return content;

def myWrite(buf):
  raw = buf.decode('utf-8').split(':')  
  checkInUrl = 'https://mis.cp.eng.chula.ac.th/krerk/teaching/2022s2-os/checkIn.php'
  parms = { 'studentid' : raw[0].encode("utf-8"), 'name' : raw[1].encode("utf-8"), 'email' : raw[2].encode("utf-8") }
  rpost = requests.post(checkInUrl, data=parms)
  return len(buf)

class MyStat(fuse.Stat):
    def __init__(self):
        self.st_mode = 0
        self.st_ino = 0
        self.st_dev = 0
        self.st_nlink = 0
        self.st_uid = 0
        self.st_gid = 0
        self.st_size = 0
        self.st_atime = 0
        self.st_mtime = 0
        self.st_ctime = 0

class MyFS(Fuse):

    def getattr(self, path):
        st = MyStat()
        if path == '/':
            st.st_mode = stat.S_IFDIR | 0o777
            st.st_nlink = 2
        elif path in particapation_path:
            st.st_mode = stat.S_IFREG | 0o444
            st.st_nlink = 1
            content = myRead()
            st.st_size = len(content)
        else:
            return -errno.ENOENT
        return st

    def readdir(self, path, offset):
        for r in  '.', '..', particapation_path[1:]:
            yield fuse.Direntry(r)

    def open(self, path, flags):
        if path != particapation_path:
            return -errno.ENOENT

    def read(self, path, size, offset):
        if path != particapation_path:
            return -errno.ENOENT
        
        content = myRead()
        slen = len(content)
        if offset < slen:
            if offset + size > slen:
                size = slen - offset
            buf = content[offset:offset+size]
        else:
            buf = b""
        return buf
    
    def write(self, path, buf, offset):
        if path != particapation_path:
            return -errno.ENOENT
        return myWrite(buf)

def main():
    usage="""
MyFS mounting_point

""" + Fuse.fusage
    server = MyFS(version="%prog " + fuse.__version__,
                     usage=usage,
                     dash_s_do="setsingle")

    server.parse(errex=1)
    server.main()

if __name__ == '__main__':
    main()
