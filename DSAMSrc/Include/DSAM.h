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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "StdMessage.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeNSpecLists.h"
#include "GeUniParMgr.h"

#include "UtAccumulate.h"
#include "UtAmpMod.h"
#include "UtBandwidth.h"
#include "UtBinSignal.h"
#include "UtDelay.h"
#include "UtDynaList.h"
#include "UtCBinauralITD.h"
#include "UtCFList.h"
#include "UtCmplxM.h"
#include "UtCompress.h"
#include "UtConvMonaural.h"
#include "UtCreateBinaural.h"
#include "UtCreateJoined.h"
#include "UtFilters.h"
#include "UtHalfWRectify.h"
#include "UtIonChanList.h"
#include "UtIterRipple.h"
#include "UtNameSpecs.h"
#include "UtOptions.h"
#include "UtPause.h"
#include "UtRamp.h"
#include "UtRandom.h"
#include "UtRedceChans.h"
#include "UtReduceDt.h"
#include "UtRefractory.h"
#include "UtSample.h"
#include "UtSelectChans.h"
#include "UtSSSymbols.h"
#include "UtSimScript.h"
#include "UtShapePulse.h"
#include "UtSpikeList.h"
/* #include "UtSSParser.h" - this shouldn't be needed by programs. */
#include "UtStddise.h"
#include "UtStrobe.h"
#include "UtUIEEEFloat.h"
#include "UtUPortableIO.h"
#include "UtString.h"

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
#include "AnModulusFT.h"
#include "AnSAImage.h"
#include "AnSpikeReg.h"
#include "AnSynchIndex.h"

#include "FiParFile.h"
#include "FiDataFile.h"
#include "FiRaw.h"
#include "FiAIFF.h"
#include "FiASCII.h"
#include "FiMSWave.h"

#include "MoANSGBinom.h"
#include "MoANSGCarney.h"
#include "MoANSGSimple.h"
#include "MoBMDRNL.h"
#include "MoBM0DRNL.h"
#include "MoBMGammaT.h"
#include "MoBMCooke.h"
#include "MoBMCarney.h"
#include "MoDendrite.h"
#include "MoHCRPCarney.h"
#include "MoHCRPMeddis.h"
#include "MoHCRPShamma.h"
#include "MoIHC86Meddis.h"
#include "MoIHC86aMeddis.h"
#include "MoIHC95Meddis.h"
#include "MoIHCCarney.h"
#include "MoIHCCooke.h"
#include "MoNCArleKim.h"
#include "MoNCHHuxley.h"
#include "MoNCMcGregor.h"
#include "MoPEBPass.h"
#include "MoPEMBPass.h"

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

#	include	"GrSignalDisp.h"

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
#/* --cplusplus ensures only included with c++ compile */
#	undef TRUE			/* required because WxWin doesn't check. */
#	undef FALSE

#	include "wx.h"
#	include "wx_form.h"
#	include "wxstring.h"

#	include "GrUtils.h"
#	include "GrFonts.h"
#	include "GrIPC.h"
#	include "GrItems.h"
#	include "GrDSAMList.h"
#	include "GrDialogInfo.h"
#	include "GrParListInfo.h"
#	include "GrParListInfoList.h"
#	include "GrDialogList.h"
#	include "GrModParDialog.h"
#	include "GrSimModDialog.h"
#	include "GrRunMgr.h"
#	include "GrRunParDialog.h"
#	include "GrSimMgr.h"
#	include "GrGraphCtrls.h"
#	include "GrLine.h"
#	include "GrCanvas.h"
#	include "GrBrushes.h"
#	include "GrDisplayS.h"

#	if !defined(DSAM_MYAPP) & !defined(DSAM_DO_NOT_DEFINE_MYAPP)
#	define DSAM_MYAPP	1
	//  This statement initialises the whole GUI application
	MyApp	myApp;
#	endif /* DSAM_MYAPP */
#endif /* GRAPHICS_SUPPORT */

#ifdef MPI_SUPPORT
#	include "mpi.h"
#	include	"PaGeneral.h"
#	include	"PaMaster1.h"
#	include	"PaUtilities.h"
#endif /* MPI_SUPPORT */

#include "GeModuleMgr.h"

#endif /* _DSAMHEADERS_H */
