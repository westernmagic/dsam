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
    """Produce numpy array from signaldata channel data which uses the same
    memory.
    
    """
    data = np.ctypeslib.as_array(sig.block, shape = (sig.numChannels,
                                                     sig.length))
    return data
def CreateSignalData(stim, dt, offset = 0, interleaveLevel =
        1, numWindowFrames = 1, outputTimeOffset = 0.0, cFs = None,
        channelTitle=u"Channel Title", chanDataTitle=u"Channel Data Title",
        sampleTitle=u"Sample Title"):
    sig = SignalData()
    sig.dtFlag = 1
    sig.lengthFlag = 1
    sig.rampFlag = 1
    sig.localInfoFlag = 1
    sig.staticTimeFlag = 0
    sig.externalDataFlag = 1
    sig.numChannels = stim.shape[0]
    sig.origNumChannels = sig.numChannels
    sig.offset = offset
    sig.interleaveLevel = interleaveLevel
    sig.numWindowFrames = 1
    sig.length = stim.shape[1]
    sig.timeIndex = 0
    sig.dt = dt
    sig.outputTimeOffset = outputTimeOffset
    sig.info = SignalInfo()
    if not cFs:
        cFs = np.arange(sig.numChannels, dtype=c_double)
    sig.info.chanLabel = np.ctypeslib.as_ctypes(cFs)
    sig.info.cFArray = np.ctypeslib.as_ctypes(cFs)
    sig.info.channelTitle = channelTitle
    sig.info.chanDataTitle = chanDataTitle
    sig.info.sampleTitle = sampleTitle
    dPtr = POINTER(c_double)
    sig.channel = (dPtr*sig.length)(*[row.ctypes.data_as(dPtr) for row in stim])
    sig.block = np.ctypeslib.as_ctypes(stim[0])
    return sig
def DBSPL2MicroPascals(dBSPL):
    mpa = 20.0 * np.sqrt(2.0) * 10 ** (dBSPL / 20.0)
    return (mpa)
def DataXAxis(sig):
    """Calculate the time range for a signal.
    
    """
    x = np.arange(sig.length) * sig.dt + sig.outputTimeOffset
    return x

