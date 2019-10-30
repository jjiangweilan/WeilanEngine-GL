import distutils.dir_util
import os
import subprocess

distutils.dir_util.copy_tree("./etc/demo", "../")
os.mkdir("../build")
os.chdir("../build")
subprocess.Popen(["cmake", ".."])


