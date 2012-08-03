from ctypes import c_wchar_p
from ctypes import POINTER
from os import environ
import numpy as np
import dsam

interfacePathEnvVar = "DSAM_GEN_INTERFACE_PATH"

interfaceName = "libRunDSAMSimGen.so"
try:
    interfacePath = environ[interfacePathEnvVar]
except KeyError:
    print "The '" + interfacePathEnvVar + "' environment variable must be set."
    raise

lib = np.ctypeslib.load_library(interfaceName, interfacePath)

libRunDSAMSim = lib.RunDSAMSim
libRunDSAMSim.argtypes = [c_wchar_p, c_wchar_p, POINTER(dsam.SignalData)]
libRunDSAMSim.restype = POINTER(dsam.SignalData)

def RunDSAMSim(sim, pars = "", inSignal = None):
    """Run a simulation producing output as a SignalData structure.

    """
    return libRunDSAMSim(sim, pars, inSignal)[0]

