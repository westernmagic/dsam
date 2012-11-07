 # -*- coding: utf-8 -*-
"""
Created on Sun Oct 07 10:44:43 2012

@author: lowel
"""

from ctypes import c_double
from ctypes import byref
from ctypes import POINTER
import matplotlib.pyplot as plt  # Matplotlib's pyplot: MATLAB-like syntax
from pylab import *              # Matplotlib's pylab interface

import dsam
from dsam.interface import PyRunDSAMSim

simFilePath = "GPDRNL57_AN057_OC039.spf"
mainPars = " diag_mode off psth_oc on"
bF = 8018.0

def PureTone(freq=1000.0, leveldB=60.0, duration=0.1, dt=1.0/44100.0):
    numChannels = 1
    amp = dsam.DBSPL2MicroPascals(leveldB)
    time = np.arange(dt, duration + dt, dt)
    sLen = len(time)
    stim = np.empty((numChannels, sLen), dtype=c_double, order="C")
    stim[0] = amp * np.sin(2.0 * pi * freq * time)
    return stim
def PlotPSTH(simFile, simPars, freq, leveldB):
    sampleRate = 100000.0
    dt = 1.0 / sampleRate
    signalDuration = 0.06
    stim = PureTone(freq, leveldB, signalDuration, dt)
    inSig = dsam.CreateSignalData(stim, dt)
    sig = PyRunDSAMSim(sim = simFile, pars = simPars, inSignal = inSig)
    data = dsam.as_array(sig)
    t = dsam.DataXAxis(sig)
    plt.plot(t, data[0,:])
    plt.title("PSTH: " + simFile)
    plt.xlabel("Time (s)")
    plt.ylabel("Spikes per bin")
def PlotRateIntensity(simFile, simPars, freq, initialInt, finalInt, deltaInt):
    steadyStateFactor = 0.9;
    sampleRate = 100000.0;
    dt = 1.0 / sampleRate;
    signalDuration = 0.1;
    levels = np.arange(initialInt, finalInt, deltaInt)
    rate = np.empty_like(levels)
    count = 0
    for leveldB in levels:
        stim = PureTone(freq, leveldB, signalDuration, dt);
        inSig = dsam.CreateSignalData(stim, dt)
        sig = PyRunDSAMSim(sim = simFile, pars = simPars, inSignal = inSig)
        data = dsam.as_array(sig)
        if (leveldB == initialInt):
            offsetSamples = sig.length * steadyStateFactor;
            offsetTime = offsetSamples * sig.dt;
            calcDuration = (sig.length - offsetSamples) * sig.dt;
        rate[count] = mean(data[0, offsetSamples:sig.length]) / calcDuration;
        count = count + 1
    plt.plot(levels, rate);
    plt.title("Rate-Intensity Function: " + simFile)
    plt.xlabel("Level (dB SPL)");
    plt.ylabel("Spike rate (sp/s)");
def RunDemo(plots = ["psth", "rif"]):
    for p in plots:
        if (p == "psth"):
            plt.figure()
            PlotPSTH(simFilePath, mainPars, bF, 80.0) 
        if (p == "rif"):
            plt.figure()
            PlotRateIntensity(simFilePath, mainPars, bF, 0, 90, 10)
    plt.show()

# Main
if __name__ == "__main__":
    RunDemo()
      


