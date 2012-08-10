from ctypes import c_wchar_p
from ctypes import POINTER
from os import environ
import numpy as np
import dsam

class info:
    def __init__(self, sig):
        self.dt = sig.dt
        self.staticTimeFlag = sig.staticTimeFlag
        self.length = sig.length
        self.numChannels = sig.numChannels 
        self.labels = np.ctypeslib.as_array(sig.info.chanLabel, (sig.numChannels, 1))
        self.numWindowFrames =  sig.numWindowFrames
        self.outputTimeOffset = sig.outputTimeOffset
        self.interleaveLevel = sig.interleaveLevel
        self.wordSize = 2.0

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

def PyRunDSAMSim(sim, pars = "", inSignal = None):
    """Run a simulation producing output as a SignalData structure.

    """
    return libRunDSAMSim(sim, pars, inSignal)[0]

def RunDSAMSim(sim, pars = "", inSignal = None):
    """Run a simulation producing 'data' and 'info' according to the
    legacy RunDSAMSim interface.

    """
    sig = libRunDSAMSim(sim, pars, inSignal)[0]
    data = dsam.as_array(sig)
    info = dsam.interface.info(sig)
    return (data, info)
