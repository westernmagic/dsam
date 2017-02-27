# Package name and version information.

import	os

interfacePathEnvVar = "DSAM_DLL_PATH"

def GetVersion():
	return "2.1.9"
def GetDSAMVersion():
	return "2.8.28"
def GetLibDir():
	if (os.name == "posix"):
	    interfacePath = "/home/lowel/git/dsam/Apps/RunDSAMSim/src/LibRunDSAMSimGen/.libs"
	else:
	    try:
	        interfacePath = os.environ[interfacePathEnvVar]
	    except KeyError:
	        print "The '" + interfacePathEnvVar + "' environment variable must be set."
	        raise
	return interfacePath

