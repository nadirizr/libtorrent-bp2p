import os
import sys


def getAllFiles(root):
    r = []
    for root, dirs, files in os.walk(sys.argv[1]):
        for x in files:
            r.append(root+"\\"+x)
    return r

files = getAllFiles(".")
hs = [x for x in  files if ".h" in x and ".svn" not in x]
os.system("griffin " + " ".join(hs))