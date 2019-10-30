import distutils.dir_util
import os
import subprocess
import errno

distutils.dir_util.copy_tree("./etc/demo", "../")
try:
    os.mkdir("../build")
except FileExistsError as err:
    pass
except OSError as other:
    raise
os.chdir("../build")
subprocess.Popen(["cmake", "..", "-DCMAKE_GENERATOR_PLATFORM=x64"])


