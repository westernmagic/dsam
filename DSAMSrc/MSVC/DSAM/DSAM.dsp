# Microsoft Developer Studio Project File - Name="DSAM" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=DSAM - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DSAM.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DSAM.mak" CFG="DSAM - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DSAM - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DSAM - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DSAM - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(DSAMDIR)\include" /I "$(DSAMDIR)" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D HAVE_CONFIG_H=1 /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\DSAM.lib"

!ELSEIF  "$(CFG)" == "DSAM - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(DSAMDIR)\include" /I "$(DSAMDIR)" /D "_DEBUG" /D HAVE_CONFIG_H=1 /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\DSAM_d.lib"

!ENDIF 

# Begin Target

# Name "DSAM - Win32 Release"
# Name "DSAM - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\MainLib\AnALSR.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\AnAutoCorr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\AnAverages.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\AnConvolve.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\AnCrossCorr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\AnExperiment.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\AnFindBin.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\AnFindNextIndex.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\AnFourierT.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\AnGeneral.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\AnHistogram.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\AnIntensity.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\AnInterSIH.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\AnSAImage.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\AnSpikeReg.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\AnSynchIndex.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\FiAIFF.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\FiASCII.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\FiDataFile.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\FiMSWave.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\FiParFile.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\FiRaw.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\FlBandPass.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\FlFIR.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\FlLowPass.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\FlMultiBPass.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\GeCommon.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\GeEarObject.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\GeModuleMgr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\GeModuleReg.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\GeNSpecLists.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\GeSignalData.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\GeUniParMgr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoANSGBinom.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoANSGCarney.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoANSGSimple.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoBM0DRNL.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoBMCarney.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoBMCooke.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoBMDRNL.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoBMGammaC.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoBMGammaT.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoHCRPCarney.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoHCRPMeddis.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoHCRPSham3StVIn.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoHCRPShamma.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoIHC86aMeddis.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoIHC86Meddis.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoIHCCarney.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoIHCCooke.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoIHCMeddis2000.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoNCArleKim.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoNCHHuxley.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\MoNCMcGregor.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\StAMTone.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\StBPTone.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\StClick.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\StEGatedTone.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\StFMTone.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\StHarmonic.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\StMPPTone.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\StMPTone.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\StPTone.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\StPTone2.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\StPulseTrain.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\StStepFun.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\StWhiteNoise.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\TrGate.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\TrSetDBSPL.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtAccumulate.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtAmpMod.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtAppInterface.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtBandwidth.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtBinSignal.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtCBinauralITD.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtCFList.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtCmplxM.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtCompress.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtConvMonaural.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtCreateBinaural.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtCreateJoined.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtDatum.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtDebug.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtDelay.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtDynaBList.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtDynaList.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtFFT.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtFilters.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtFIRFilters.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtGCFilters.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtHalfWRectify.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtIonChanList.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtIterRipple.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtLocalChans.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtNameSpecs.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtOptions.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtPadSignal.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtParArray.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtPause.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtRamp.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtRandom.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtRedceChans.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtReduceDt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtRefractory.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtRemez.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtSample.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtSelectChans.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtShapePulse.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtSimScript.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtSpikeList.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtSSParser.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtSSSymbols.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtStddise.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtString.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtStrobe.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtSwapLR.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtUIEEEFloat.c
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtUPortableIO.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\Include\AnALSR.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnAutoCorr.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnAverages.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnConvolve.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnCrossCorr.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnExperiment.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnFindBin.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnFindNextIndex.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnFourierT.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnGeneral.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnHistogram.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnIntensity.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnInterSIH.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnSAImage.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnSpikeReg.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnSynchIndex.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\DSAM.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FiAIFF.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FiASCII.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FiDataFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FiMSWave.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FiParFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FiRaw.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FlBandPass.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FlFIR.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FlLowPass.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FlMultiBPass.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GeCommon.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GeEarObject.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GeModuleMgr.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GeModuleReg.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GeNSpecLists.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GeSignalData.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GeUniParMgr.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrAxisScale.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrBrushes.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrCanvas.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrDialogInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrDialogList.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrDisplayS.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrDSAMList.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrFonts.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrLine.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrModParDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrParControl.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrParListInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrParListInfoList.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrPrintDisp.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrSignalDisp.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrSimMgr.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrSimModDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrSimThread.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrSocket.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GrUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoANSGBinom.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoANSGCarney.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoANSGSimple.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoBM0DRNL.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoBMCarney.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoBMCooke.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoBMDRNL.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoBMGammaC.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoBMGammaT.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoHCRPCarney.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoHCRPMeddis.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoHCRPSham3StVIn.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoHCRPShamma.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoIHC86aMeddis.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoIHC86Meddis.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoIHCCarney.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoIHCCooke.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoIHCMeddis2000.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoNCArleKim.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoNCHHuxley.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MoNCMcGregor.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\StAMTone.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\StBPTone.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\StClick.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\StdMessage.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\StdSimThread.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\StEGatedTone.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\StFMTone.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\StHarmonic.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\StMPPTone.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\StMPTone.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\StPTone.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\StPTone2.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\StPulseTrain.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\StStepFun.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\StWhiteNoise.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\TrGate.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\TrSetDBSPL.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtAccumulate.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtAmpMod.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtAppInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtBandwidth.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtBinSignal.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtCBinauralITD.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtCFList.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtCmplxM.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtCompress.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtConvMonaural.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtCreateBinaural.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtCreateJoined.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtDatum.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtDebug.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtDelay.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtDynaBList.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtDynaList.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtFFT.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtFilters.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtFIRFilters.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtGCFilters.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtHalfWRectify.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtIonChanList.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtIterRipple.h
# End Source File
# Begin Source File

SOURCE=\projects\dsam\DSAMSrc\Include\UtLocalChans.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtNameSpecs.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtOptions.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtPadSignal.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtParArray.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtPause.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtRamp.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtRandom.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtRedceChans.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtReduceDt.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtRefractory.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtRemez.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtSample.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtSelectChans.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtShapePulse.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtSimScript.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtSpikeList.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtSSParser.h
# End Source File
# Begin Source File

SOURCE=..\..\src\MainLib\UtSSParser.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtSSSymbols.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtStddise.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtString.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtStrobe.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtSwapLR.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtUIEEEFloat.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtUPortableIO.h
# End Source File
# End Group
# End Target
# End Project
