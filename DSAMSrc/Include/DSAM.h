/**********************
 *
 * File:		DSAM.h
 * Purpose:		This file includes all of the headers required for using the
 *				DSAM Core Routines Library
 * Comments:	It can be used to avoid missing vital header files, though
 *				header files may still be included individually.
 *				16-04-99 LPO: Introduced the 'myApp' declaration to solve some
 *				linking problems with shared libraries on IRIX 6.2.
 * Author:		Lowel P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		16 Apr 1999
 * Copyright:	(c) 1999, University of Essex
 *
 *********************/

#ifndef _DSAMHEADERS_H
#define	_DSAMHEADERS_H 1

#include <stdio.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "StdMessage.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeNSpecLists.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GeModuleReg.h"

#include "StdSimThread.h"

#include "UtAccumulate.h"
#include "UtAmpMod.h"
#include "UtAppInterface.h"
#include "UtBandwidth.h"
#include "UtBinSignal.h"
#include "UtDelay.h"
#include "UtDynaBList.h"
#include "UtDynaList.h"
#include "UtCBinauralITD.h"
#include "UtCFList.h"
#include "UtCmplxM.h"
#include "UtCompress.h"
#include "UtConvMonaural.h"
#include "UtCreateBinaural.h"
#include "UtCreateJoined.h"
#include "UtFilters.h"
#include "UtGCFilters.h"
#include "UtFIRFilters.h"
#include "UtFFT.h"
#include "UtHalfWRectify.h"
#include "UtIonChanList.h"
#include "UtIterRipple.h"
#include "UtLocalChans.h"
#include "UtNameSpecs.h"
#include "UtOptions.h"
#include "UtPadSignal.h"
#include "UtPause.h"
#include "UtRamp.h"
#include "UtRandom.h"
#include "UtRedceChans.h"
#include "UtReduceDt.h"
#include "UtRefractory.h"
#include "UtRemez.h"
#include "UtSample.h"
#include "UtSelectChans.h"
#include "UtSSSymbols.h"
#include "UtSimScript.h"
#include "UtShapePulse.h"
#include "UtSpikeList.h"
#include "UtStddise.h"
#include "UtStrobe.h"
#include "UtUIEEEFloat.h"
#include "UtUPortableIO.h"
#include "UtString.h"
#include "UtSwapLR.h"
#include "UtParArray.h"

#include "AnALSR.h"
#include "AnAutoCorr.h"
#include "AnAverages.h"
#include "AnConvolve.h"
#include "AnCrossCorr.h"
#include "AnExperiment.h"
#include "AnFourierT.h"
#include "AnFindNextIndex.h"
#include "AnFindBin.h"
#include "AnGeneral.h"
#include "AnHistogram.h"
#include "AnIntensity.h"
#include "AnInterSIH.h"
#include "AnSAImage.h"
#include "AnSpikeReg.h"
#include "AnSynchIndex.h"

#include "FiParFile.h"
#include "FiDataFile.h"
#include "FiRaw.h"
#include "FiAIFF.h"
#include "FiASCII.h"
#include "FiMSWave.h"

#include "FlBandPass.h"
#include "FlFIR.h"
#include "FlLowPass.h"
#include "FlMultiBPass.h"

#include "MoANSGBinom.h"
#include "MoANSGCarney.h"
#include "MoANSGSimple.h"
#include "MoBMDRNL.h"
#include "MoBM0DRNL.h"
#include "MoBMGammaC.h"
#include "MoBMGammaT.h"
#include "MoBMCooke.h"
#include "MoBMCarney.h"
#include "MoHCRPCarney.h"
#include "MoHCRPMeddis.h"
#include "MoHCRPSham3StVIn.h"
#include "MoHCRPShamma.h"
#include "MoIHC86Meddis.h"
#include "MoIHC86aMeddis.h"
#include "MoIHCMeddis2000.h"
#include "MoIHCCarney.h"
#include "MoIHCCooke.h"
#include "MoNCArleKim.h"
#include "MoNCHHuxley.h"
#include "MoNCMcGregor.h"

#include "StAMTone.h"
#include "StBPTone.h"
#include "StClick.h"
#include "StFMTone.h"
#include "StEGatedTone.h"
#include "StHarmonic.h"
#include "StMPTone.h"
#include "StMPPTone.h"
#include "StPTone.h"
#include "StPulseTrain.h"
#include "StPTone2.h"
#include "StStepFun.h"
#include "StWhiteNoise.h"

#include "TrGate.h"
#include "TrSetDBSPL.h"

#include "GrSignalDisp.h"

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
#/* --cplusplus ensures only included with c++ compile */
#	undef TRUE			/* required because WxWin doesn't check. */
#	undef FALSE

// For compilers that support precompilation, includes "wx.h".
#	include <wx/wxprec.h>

#	ifdef __BORLANDC__
	    #pragma hdrstop
#	endif

// Any files you want to include if not precompiling by including
// the whole of <wx/wx.h>
#	ifndef WX_PRECOMP
#		include <wx/wx.h>
#	endif

// Any files included regardless of precompiled headers
#	include <wx/notebook.h>
#	include <wx/print.h>
#	include "wx/printdlg.h"
#	include "wx/socket.h"

#	if wxTEST_POSTSCRIPT_IN_MSW
#		include "wx/generic/printps.h"
#		include "wx/generic/prntdlgg.h"
#	endif

#	include "GrUtils.h"
#	include "GrFonts.h"
#	include "GrDialogInfo.h"
#	include "GrParControl.h"
#	include "GrParListInfo.h"
#	include "GrParListInfoList.h"
#	include "GrDialogList.h"
#	include "GrModParDialog.h"
#	include "GrSimModDialog.h"
#	include "GrSimThread.h"
#	include "GrSimMgr.h"
#	include "GrLines.h"
#	include "GrAxisScale.h"
#	include "GrCanvas.h"
#	include "GrPrintDisp.h"
#	include "GrBrushes.h"
#	include "GrDisplayS.h"

#endif /* GRAPHICS_SUPPORT */

#ifdef MPI_SUPPORT
#	include "mpi.h"
#	include	"PaGeneral.h"
#	include	"PaMaster1.h"
#	include	"PaUtilities.h"
#endif /* MPI_SUPPORT */

#endif /* _DSAMHEADERS_H */

