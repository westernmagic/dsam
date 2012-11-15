#!/usr/bin/python

import os.path
import subprocess
import sys

pathList = subprocess.check_output("find /usr/lib/jvm -name " + sys.argv[1] +
  " -print", shell=True)
filePath = pathList.partition("\n")[0]
dirPath = os.path.dirname(filePath)

print dirPath
