from ctypes import POINTER
from ctypes import c_wchar
from ctypes import c_short
from ctypes import c_int
from ctypes import c_double
from ctypes import c_ushort
from ctypes import c_ulong
from ctypes import Structure
import numpy as np

SIGNALDATA_MAX_TITLE = 80

class SignalInfo(Structure):
    _fields_ = [
        ("chanLabel", POINTER(c_double)),
        ("cFArray", POINTER(c_double)),
        ("channelTitle", c_wchar * SIGNALDATA_MAX_TITLE),
        ("chanDataTitle", c_wchar * SIGNALDATA_MAX_TITLE),
        ("sampleTitle", c_wchar * SIGNALDATA_MAX_TITLE)
    ]
    
class SignalData(Structure):
    _fields_ = [
        ("dtFlag", c_int),
        ("lengthFlag", c_int),
        ("rampFlag", c_int),
        ("localInfoFlag", c_int),
        ("staticTimeFlag", c_int),
        ("externalDataFlag", c_int),
        ("numChannels", c_ushort),
        ("origNumChannels", c_ushort),
        ("offset", c_ushort),
        ("interleaveLevel", c_ushort),
        ("numWindowFrames", c_ushort),
        ("length", c_ulong),
        ("timeIndex", c_ulong),
        ("dt", c_double),
        ("outputTimeOffset", c_double),
        ("info", SignalInfo),
        ("channel", POINTER(POINTER(c_double))),
        ("block", POINTER(c_double))
    ]

def as_array(sig):
    """Produce numpy array from signaldata channel data which uses the same memory.
    
    """
    data = np.ctypeslib.as_array(sig.block, (sig.length, sig.numChannels))
    return data
def DataXAxis(sig):
    """Calculate the time range for a signal.
    
    """
    x = np.arange(sig.outputTimeOffset, sig.outputTimeOffset + sig.dt * sig.length,
            sig.dt)
    return x

