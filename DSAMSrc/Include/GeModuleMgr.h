/******************
 *
 * File:		GeModuleMgr.h
 * Comments:	These routines carry out various functions such as freeing
 *				all module memory space, and so on.
 *				17-04-98 LPO: Added the NULL_MODULE_NAME string constant.
 *				22-04-98 LPO: Changed all the stimulus module names so that they
 *				start with "Stim_".
 *				10-12-98 LPO: Changed the old 'NullFunction_...' to
 *				'TrueFunction_...' which alwaysreturns a TRUE value.  I have
 *				created a new 'NullFunction_...' which always returns the
 *				correct NULL (zero) value.
 *				05-03-99 LPO: The 'SetParsPointer_' routine now checks for a
 *				NULL module passed as an argument.
 *				18-03-99 LPO: I have added the 'LinkGlueRoutine'.  This routine
 *				is not meant to be used, but it is to access global variables
 *				of modules that wouldn't otherwise be linked.
 *				20-04-99 LPO: The "Free_ModuleMgr" routine was not setting the
 *				appropriate module parameters pointer using the 
 *				"SetParsPointer_" routine.
 *				27-04-99 LPO:  The universal parameter lists have allowed me
 *				to introduce the 'ReadPar_', 'PrintPar_' and 'RunProcess'
 *				'SetPar_' commands.  These commands replace the previous
 *				generic programming interface.  It will also mean that the
 *				ModuleMgr module can be automatically generated.
 *				10-05-99 LPO: Introduced the 'ReadSimParFile_' routine.  There
 *				is only one module which uses this routine, but since it may
 *				be used quite alot in programs, it is best to put it in.
 *				02-06-99 LPO: In line with my drive to remove the 'DoFun'
 *				interface I have created the 'SetRealPar_', 'GetCFListPtr_' and
 *				'GetSimulation_' routines.  These routines will only work with
 *				modules that have these functions.
 *				22-10-99 LPO: The global 'nullModule' is initialised by the
 *				first call to 'Init_'.  Only one instance is ever initialised.
 * Authors:		L. P. O'Mard
 * Created:		29 Mar 1993
 * Updated:		02 Jun 1999
 * Copyright:	(c) 1999, University of Essex.
 * 
 ******************/

#ifndef	_MODULEMGR_H
#define _MODULEMGR_H	1

#include "UtNameSpecs.h"
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	LOCAL_PARAMETERS		TRUE	/* For setting mod. parameter pointer */
#define	GLOBAL_PARAMETERS		FALSE	/* For setting mod. parameter pointer */
#define MAX_MODULE_NAME			30	/* Max. characters in module name. */
#define	NULL_MODULE_NAME		"NULL"
#define BM_MODULE_PREFIX		"BM_"

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

/*************************** Misc. Macros *************************************/

#define MODULE_SPECIFIER(EAROBJ)	((EAROBJ)->module->specifier)

#define MODULE_NAME(EAROBJ)			((EAROBJ)->module->name)

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {
	
	ANA_ACF,
	ANA_AVERAGES,
	ANA_CCF,
	ANA_CONVOLUTION,
	ANA_FINDBIN,
	ANA_FINDNEXTINDEX,
	ANA_FOURIERT,
	ANA_HISTOGRAM,
	ANA_INTENSITY,
	ANA_ISIH,
	ANA_SAI,
	ANA_SPIKEREGULARITY,
	ANA_SYNCHINDEX,
	AN_SG_BINOMIAL,
	AN_SG_CARNEY,
	AN_SG_SIMPLE,
	BM_CARNEY,
	BM_COOKE,
	BM_GAMMAT,
	BM_DRNL,
	BM_DRNL_TEST,
	DATAFILE_IN,
	DATAFILE_OUT,
	DISPLAY_SIGNAL,
	FILT_BANDPASS,
	FILT_LOWPASS,
	FILT_MULTIBPASS,
	IHC_CARNEY,
	IHC_COOKE91,
	IHC_MEDDIS86,
	IHC_MEDDIS86A,
	IHC_MEDDIS2000,
	IHCRP_CARNEY,
	IHCRP_MEDDIS,
	IHCRP_SHAMMA,
	NEUR_ARLEKIM,
	NEUR_HHUXLEY,
	NEUR_MCGREGOR,
	NULL_MODULE,
	MPI_MASTER1,
	STIM_CLICK,
	STIM_EXPGATEDTONE,
	STIM_HARMONIC,
	STIM_PULSETRAIN,
	STIM_PURETONE,
	STIM_PURETONE_2,
	STIM_PURETONE_AM,
	STIM_PURETONE_BINAURAL,
	STIM_PURETONE_FM,
	STIM_PURETONE_MULTI,
	STIM_PURETONE_MULTIPULSE,
	STIM_STEPFUN,
	STIM_WHITENOISE,
	TRANS_GATE,
	TRANS_SETDBSPL,
	UTIL_ACCUMULATE,
	UTIL_AMPMOD,
	UTIL_BINSIGNAL,
	UTIL_CONVMONAURAL,
	UTIL_COMPRESSION,
	UTIL_CREATEBINAURAL,
	UTIL_CREATEBINAURALITD,
	UTIL_CREATEJOINED,
	UTIL_DELAY,
	UTIL_HALFWAVERECTIFY,
	UTIL_ITERATEDRIPPLE,
	UTIL_PAUSE,
	UTIL_REDUCECHANNELS,
	UTIL_REDUCEDT,
	UTIL_REFRACTORYADJUST,
	UTIL_SAMPLE,
	UTIL_SELECTCHANNELS,
	UTIL_SHAPEPULSE,
	UTIL_SIMSCRIPT,
	UTIL_STANDARDISE,
	UTIL_STROBE,

	NULL_SPECIFIER

} ModuleSpecifier;

typedef unsigned int	ModuleHandle;	/* For module management. */

typedef struct moduleStruct {

	BOOLN	onFlag;
	char	name[MAX_MODULE_NAME];
	ModuleSpecifier	specifier;
	ModuleHandle	handle;				/* Reference handle for manager. */
	
	/* General, accessible functions. */
	BOOLN	(* CheckData)(EarObjectPtr data);
	BOOLN	(* CheckPars)(void);
	BOOLN	(* Free)(void);
	CFListPtr	(* GetCFListPtr)(void);
	IonChanListPtr	(* GetICListPtr)(void);
	double	(* GetIndividualFreq)(int index);
	double	(* GetIndividualIntensity)(int index);
	double	(* GetPotentialResponse)(double potential);
	int		(* GetNumWorkers)(void);
	double	(* GetRestingResponse)(void);
	DatumPtr	(* GetSimulation)(void);
	UniParListPtr	(* GetUniParListPtr)(void);
	BOOLN	(* Init)(ParameterSpecifier parSpec);
	BOOLN	(* PrintPars)(void);
	BOOLN	(* PrintSimParFile)(void);
#	ifdef _PAMASTER1_H
		BOOLN (* QueueCommand)(void *parameter, int parCount,
		  TypeSpecifier type, char *label, CommandSpecifier command,
		  ScopeSpecifier scope);
		BOOLN (* SendQueuedCommands)(void);
#	endif
	BOOLN	(* RunProcess)(EarObjectPtr theObject);
	BOOLN	(* ReadPars)(char *fileName);
	BOOLN	(* ReadSignal)(char *fileName, EarObjectPtr data);
	BOOLN	(* ReadSimParFile)(char *fileName);
	BOOLN	(* WriteOutSignal)(char *fileName, EarObjectPtr data);
	
	/* Module Parameters */
	union {

		AmpMod		ampMod;
		AMTone		aMTone;
		ArleKim		arleKim;
		AutoCorr	autoCorr;
		Averages	averages;
		BinomialSG	binomialSG;
		BinSignal	binSignal;
		BMGammaT	bMGammaT;
		BM0DRNL		bM0DRNL;
		BM0Cooke	bM0Cooke;
		BMCarney	bMCarney;
		BMDRNL		bMDRNL;
		BPureTone	bPureTone;
		CarneySG	carneySG;
		CarneyRP	carneyRP;
		CarneyHC	carneyHC;
		Click		click;
		CookeHC		cookeHC;
		Compression	compression;
		CreateBinITD	createBinITD;
		CrossCorr	crossCorr;
		DataFile	dataFile;
		LowPassF	lowPassF;
		EGatedTone	eGatedTone;
		FMTone		fMTone;
		FindBin		findBin;
		FindIndex	findIndex;
		FourierT	fourierT;
		Gate		gate;
		HairCell	hairCell;
		HairCell2	hairCell2;
		HairCell3	hairCell3;
		Harmonic	harmonic;
		HHuxleyNC	hHuxleyNC;
		Histogram	histogram;
		Intensity	intensity;
		InterSIH	interSIH;
		Delay2	delay2;
		IterRipple	iterRipple;

#		ifdef _PAMASTER1_H
		Master1		master1;
#		endif

		McGregor	mcGregor;
		MeddisRP	meddisRP;
		MPureTone	mPureTone;
		Pause		pause;
		MultiBPassF	multiBPassF;
		BandPassF	bandPassF;
		PulseTrain	pulseTrain;
		PureTone	pureTone;
		PureTone4	pureTone4;
		PureTone2	pureTone2;
		ReduceChans	reduceChans;
		ReduceDt	reduceDt;
		RefractAdj	refractAdj;
		SAImage		sAImage;
		Sample		sample;
		SelectChan	selectChan;
		SetDBSPL	setDBSPL;
		Shamma		shamma;
		ShapePulse	shapePulse;
		SignalDisp	signalDisp;
		SimpleSG	simpleSG;
		SimScript	simScript;
		SpikeReg	spikeReg;
		StepFun		stepFun;
		Strobe		strobe;
		WhiteNoise	whiteNoise;

	} pars;

} Module;

#ifndef MODULE_PTR
#	define MODULE_PTR

	typedef struct moduleStruct  *ModulePtr;/* Pre-referencing of module.  */
	
#endif

typedef struct moduleNode {

	ModulePtr			module;
	struct moduleNode	*next;
	
} ModuleRef, *ModuleRefPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN		AddModuleRef_ModuleMgr(ModuleRefPtr *theList, ModulePtr theModule);

BOOLN		CheckData_ModuleMgr(EarObjectPtr data, const char *callingFunction);

ModuleRefPtr	CreateModuleRef_ModuleMgr(ModulePtr theModule);

BOOLN		Enable_ModuleMgr(EarObjectPtr data, BOOLN on);

void		Free_ModuleMgr(ModulePtr *theModule);

void		FreeAll_ModuleMgr(void);

void		FreeModuleRef_ModuleMgr(ModuleRefPtr *theList, ModuleHandle
			  theHandle);

CFListPtr	GetCFListPtr_ModuleMgr(EarObjectPtr data);

double		GetPotentialResponse_ModuleMgr(EarObjectPtr data, double potential);

double		GetRestingResponse_ModuleMgr(EarObjectPtr data);

DatumPtr	GetSimulation_ModuleMgr(EarObjectPtr data);

UniParPtr	GetUniParPtr_ModuleMgr(EarObjectPtr data, char *parName);

ModulePtr	Init_ModuleMgr(char *name);

void		LinkGlueRoutine_ModuleMgr(void);

void *		NoFunction_ModuleMgr(void);

void *		NullFunction_ModuleMgr(void);

BOOLN		PrintPars_ModuleMgr(EarObjectPtr data);

BOOLN		PrintSimParFile_ModuleMgr(EarObjectPtr data);

BOOLN		ReadPars_ModuleMgr(EarObjectPtr data, char *fileName);

BOOLN		ReadSimParFile_ModuleMgr(EarObjectPtr data, char *simParFileName);

BOOLN		RunProcess_ModuleMgr(EarObjectPtr data);

BOOLN		RunModel_ModuleMgr_Null(EarObjectPtr data);

void		SetANSpikeGen_Binomial_ModuleMgr(ModulePtr theModule);

void		SetANSpikeGen_Carney_ModuleMgr(ModulePtr theModule);

void		SetANSpikeGen_Simple_ModuleMgr(ModulePtr theModule);

void		SetAnalysis_ACF_ModuleMgr(ModulePtr theModule);

void		SetAnalysis_Convolution_ModuleMgr(ModulePtr theModule);

void		SetAnalysis_Averages_ModuleMgr(ModulePtr theModule);

void		SetAnalysis_CCF_ModuleMgr(ModulePtr theModule);

void		SetAnalysis_FindBin_ModuleMgr(ModulePtr theModule);

void		SetAnalysis_FindNextIndex_ModuleMgr(ModulePtr theModule);

void		SetAnalysis_FourierT_ModuleMgr(ModulePtr theModule);

void		SetAnalysis_Histogram_ModuleMgr(ModulePtr theModule);

void		SetAnalysis_Intensity_ModuleMgr(ModulePtr theModule);

void		SetAnalysis_ISIH_ModuleMgr(ModulePtr theModule);

void		SetAnalysis_SAI_ModuleMgr(ModulePtr theModule);

void		SetAnalysis_SpikeReg_ModuleMgr(ModulePtr theModule);

void		SetAnalysis_SynchIndex_ModuleMgr(ModulePtr theModule);

void		SetAMTone_ModuleMgr(ModulePtr theModule);

void		SetBasilarM_Carney_ModuleMgr(ModulePtr theModule);

void		SetBasilarM_Cooke_ModuleMgr(ModulePtr theModule);

void		SetBasilarM_GammaT_ModuleMgr(ModulePtr theModule);

void		SetBasilarM_DRNL_ModuleMgr(ModulePtr theModule);

void		SetBasilarM_DRNL_Test_ModuleMgr(ModulePtr theModule);

void		SetBPureTone_ModuleMgr(ModulePtr theModule);

void		SetClick_ModuleMgr(ModulePtr theModule);

void		SetDataFileIn_ModuleMgr(ModulePtr theModule);

void		SetDataFileOut_ModuleMgr(ModulePtr theModule);

void		SetDefault_ModuleMgr(ModulePtr module, void *(* DefaultFunc)(void));

void		SetFMTone_ModuleMgr(ModulePtr theModule);

void		SetHarmonic_ModuleMgr(ModulePtr theModule);

void		SetIHC_Cooke91_ModuleMgr(ModulePtr theModule);

void		SetIHC_Carney_ModuleMgr(ModulePtr theModule);

void		SetIHC_Meddis86_ModuleMgr(ModulePtr theModule);

void		SetIHC_Meddis86a_ModuleMgr(ModulePtr theModule);

void		SetIHC_Meddis2000_ModuleMgr(ModulePtr theModule);

void		SetIHCRP_Carney_ModuleMgr(ModulePtr theModule);

void		SetIHCRP_Meddis_ModuleMgr(ModulePtr theModule);

void		SetIHCRP_Shamma_ModuleMgr(ModulePtr theModule);

void		SetMPureTone_ModuleMgr(ModulePtr theModule);

void		SetNeuron_ArleKim_ModuleMgr(ModulePtr theModule);

void		SetFilter_LowPass_ModuleMgr(ModulePtr theModule);

void		SetNeuron_HHuxley_ModuleMgr(ModulePtr theModule);

void		SetNeuron_McGregor_ModuleMgr(ModulePtr theModule);

void		SetNull_ModuleMgr(ModulePtr module);

void		SetMPPureTone_ModuleMgr(ModulePtr theModule);

void		SetMPI_Master1_ModuleMgr(ModulePtr theModule);

BOOLN		SetPar_ModuleMgr(EarObjectPtr data, char *parName, char *value);

BOOLN		SetParsPointer_ModuleMgr(ModulePtr theModule);

void		SetFilter_BandPass_ModuleMgr(ModulePtr theModule);

void		SetFilter_MultiBPass_ModuleMgr(ModulePtr theModule);

void		SetPulseTrain_ModuleMgr(ModulePtr theModule);

void		SetPureTone_ModuleMgr(ModulePtr theModule);

void		SetPureTone2_ModuleMgr(ModulePtr theModule);

BOOLN		SetRealArrayPar_ModuleMgr(EarObjectPtr data, char *name, int index,
			  double value);

BOOLN		SetRealPar_ModuleMgr(EarObjectPtr data, char *name, double value);

void		SetSignalDisp_ModuleMgr(ModulePtr theModule);

void		SetStepFun_ModuleMgr(ModulePtr theModule);

void		SetStimulus_ExpGatedTone_ModuleMgr(ModulePtr theModule);

void		SetTransform_Gate_ModuleMgr(ModulePtr theModule);

void		SetTransform_SetDBSPL_ModuleMgr(ModulePtr theModule);

void		SetUtility_Accumulate_ModuleMgr(ModulePtr theModule);

void		SetUtility_AmpMod_ModuleMgr(ModulePtr theModule);

void		SetUtility_BinSignal_ModuleMgr(ModulePtr theModule);

void		SetUtility_Compression_ModuleMgr(ModulePtr theModule);

void		SetUtility_ConvMonaural_ModuleMgr(ModulePtr theModule);

void		SetUtility_CreateBinaural_ModuleMgr(ModulePtr theModule);

void		SetUtility_CreateBinauralITD_ModuleMgr(ModulePtr theModule);

void		SetUtility_CreateJoined_ModuleMgr(ModulePtr theModule);

void		SetUtility_Delay_ModuleMgr(ModulePtr theModule);

void		SetUtility_HalfWaveRectify_ModuleMgr(ModulePtr theModule);

void		SetUtility_IteratedRipple_ModuleMgr(ModulePtr theModule);

void		SetUtility_Pause_ModuleMgr(ModulePtr theModule);

void		SetUtility_ReduceChannels_ModuleMgr(ModulePtr theModule);

void		SetUtility_ReduceDt_ModuleMgr(ModulePtr theModule);

void		SetUtility_RefractoryAdjust_ModuleMgr(ModulePtr theModule);

void		SetUtility_Sample_ModuleMgr(ModulePtr theModule);

void		SetUtility_SelectChannels_ModuleMgr(ModulePtr theModule);

void		SetUtility_ShapePulse_ModuleMgr(ModulePtr theModule);

void		SetUtility_SimScript_ModuleMgr(ModulePtr theModule);

void		SetUtility_Standardise_ModuleMgr(ModulePtr theModule);

void		SetUtility_Strobe_ModuleMgr(ModulePtr theModule);

void		SetWhiteNoise_ModuleMgr(ModulePtr theModule);

void *		TrueFunction_ModuleMgr(void);

__END_DECLS

#endif
