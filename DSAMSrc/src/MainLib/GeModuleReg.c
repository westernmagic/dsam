/******************
 *
 * File:		GeModuleReg.c
 * Comments:	This module handles the register of all the library and user
 *				process modules.
 *				It is necssary to use a module specifier to identify
 *				non-standard process modules such as SIMSCRIPT_MODULE and the
 *				NULL_MODULE
 * Authors:		L. P. O'Mard
 * Created:		29 Mar 1993
 * Updated:		
 * Copyright:	(c) 2001, University of Essex.
 * 
 ******************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "DSAM.h"

/******************************************************************************/
/************************** Global Variables **********************************/
/******************************************************************************/

int			numUserModules, maxUserModules;
ModRegEntry	*userModuleList = NULL;

/******************************************************************************/
/************************** Subroutines and functions *************************/
/******************************************************************************/

/************************** LibraryList ***************************************/

/*
 * This routine returns an entry to the module library list.
 */

ModRegEntryPtr
LibraryList_ModuleReg(uShort index)
{
	static ModRegEntry	moduleList[] = {

	{ "ANA_ACF",
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_ACF },
	{ "ANA_ALSR",
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_ALSR },
	{ "ANA_AVERAGES",
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_Averages },
	{ "ANA_CCF",
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_CCF },
	{ "ANA_CONVOLUTION",
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_Convolution },
	{ "ANA_FINDBIN",
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_FindBin },
	{ "ANA_FINDNEXTINDEX",
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_FindNextIndex },
	{ "ANA_FOURIERT",
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_FourierT },
	{ "ANA_HISTOGRAM",
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_Histogram },
	{ "ANA_INTENSITY",
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_Intensity },
	{ "ANA_ISIH",
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_ISIH },
	{ "ANA_SAI",
		ANALYSIS_MODULE_CLASS, ANA_SAI_MODULE,
		InitModule_Analysis_SAI },
	{ "ANA_SYNCHINDEX",
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_SynchIndex },
	{ "ANA_SPIKEREGULARITY",
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_SpikeRegularity },
	{ "AN_SG_BINOMIAL",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_ANSpikeGen_Binomial },
	{ "AN_SG_CARNEY",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_ANSpikeGen_Carney },
	{ "AN_SG_MEDDIS02",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_ANSpikeGen_Meddis02 },
	{ "AN_SG_SIMPLE",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_ANSpikeGen_Simple },
	{ "BM_CARNEY",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_BasilarM_Carney },
	{ "BM_COOKE",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_BasilarM_Cooke },
	{ "BM_DRNL",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_BasilarM_DRNL },
	{ "BM_DRNL_TEST",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_BasilarM_DRNL_Test },
	{ "BM_GAMMAC",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_BasilarM_GammaChirp },
	{ "BM_GAMMAT",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_BasilarM_GammaT },
	{ "BM_ZHANG",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_BasilarM_Zhang },
	{ DATAFILE_IN_MOD_NAME,
		IO_MODULE_CLASS, DATAFILE_MODULE,
		InitModule_DataFile },
	{ DATAFILE_OUT_MOD_NAME,
		IO_MODULE_CLASS, DATAFILE_MODULE,
		InitModule_DataFile },
	{ "DISPLAY_SIGNAL",
		DISPLAY_MODULE_CLASS, DISPLAY_MODULE,
		InitModule_SignalDisp },
	{ "FILT_BANDPASS",
		FILTER_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Filter_BandPass },
	{ "FILT_FIR",
		FILTER_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Filter_FIR },
	{ "FILT_LOWPASS",
		FILTER_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Filter_LowPass },
	{ "FILT_MULTIBPASS",
		FILTER_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Filter_MultiBPass },
	{ "IHC_CARNEY",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHC_Carney },
	{ "IHC_COOKE91",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHC_Cooke91 },
	{ "IHC_MEDDIS86",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHC_Meddis86 },
	{ "IHC_MEDDIS86A",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHC_Meddis86a },
	{ "IHC_MEDDIS2000",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHC_Meddis2000 },
	{ "IHC_ZHANG",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHC_Zhang },
	{ "IHCRP_CARNEY",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHCRP_Carney },
	{ "IHCRP_MEDDIS",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHCRP_Meddis },
	{ "IHCRP_SHAMMA",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHCRP_Shamma },
	{ "IHCRP_SHAMMA3StateVelIn",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
	  InitModule_IHCRP_Shamma3StateVelIn },
	{ "IHCRP_ZHANG",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHCRP_Zhang },
	{ "NEUR_ARLEKIM",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Neuron_ArleKim },
	{ "NEUR_HHUXLEY",
		MODEL_MODULE_CLASS, NEUR_HHUXLEY_MODULE,
		InitModule_Neuron_HHuxley },
	{ "NEUR_MCGREGOR",
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Neuron_McGregor },
	{ "NULL",
		CONTROL_MODULE_CLASS, NULL_MODULE,
		SetNull_ModuleMgr },
#	ifdef _PAMASTER1_H
	{ "MPI_MASTER1",
		CONTROL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_MPI_Master1 },
#	endif
	{ "STIM_CLICK",
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Click },
	{ "STIM_EXPGATEDTONE",
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Stimulus_ExpGatedTone },
	{ "STIM_HARMONIC",
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Harmonic },
	{ "STIM_PULSETRAIN",
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_PulseTrain },
	{ "STIM_PURETONE",
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_PureTone },
	{ "STIM_PURETONE_2",
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_PureTone_2 },
	{ "STIM_PURETONE_AM",
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_PureTone_AM },
	{ "STIM_PURETONE_BINAURAL",
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_PureTone_Binaural },
	{ "STIM_PURETONE_FM",
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_PureTone_FM },
	{ "STIM_PURETONE_MULTI",
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_PureTone_Multi },
	{ "STIM_PURETONE_MULTIPULSE",
		IO_MODULE_CLASS, PROCESS_MODULE,
	  InitModule_PureTone_MultiPulse },
	{ "STIM_STEPFUN",
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_StepFunction },
	{ "STIM_WHITENOISE",
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_WhiteNoise },
	{ "TRANS_COLLECTSIGNALS",
		TRANSFORM_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Transform_CollectSignals },
	{ "TRANS_DAMP", TRANSFORM_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Transform_Gate },
	{ "TRANS_GATE",
		TRANSFORM_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Transform_Gate },
	{ "TRANS_RAMP", TRANSFORM_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Transform_Gate },
	{ "TRANS_SETDBSPL",
		TRANSFORM_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Transform_SetDBSPL },
	{ "UTIL_ACCUMULATE",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_Accumulate },
	{ "UTIL_AMPMOD",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_AmpMod },
	{ "UTIL_BINSIGNAL",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_BinSignal },
	{ "UTIL_COMPRESSION",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_Compression },
	{ "UTIL_CONVMONAURAL",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_ConvMonaural },
	{ "UTIL_CREATEBINAURAL",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_CreateBinaural },
	{ "UTIL_CREATEBINAURALITD",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
	  InitModule_Utility_CreateBinauralITD },
	{ "UTIL_CREATEJOINED",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_CreateJoined },
	{ "UTIL_DELAY",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_Delay },
	{ "UTIL_HALFWAVERECTIFY",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_HalfWaveRectify },
	{ "UTIL_ITERATEDRIPPLE",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_IteratedRipple },
	{ "UTIL_LOCALCHANS",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_LocalChans },
	{ "UTIL_MATHOP",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_MathOp },
	{ "UTIL_PADSIGNAL",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_PadSignal },
	{ "UTIL_PAUSE",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_Pause },
	{ "UTIL_REDUCECHANNELS",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_ReduceChannels },
	{ "UTIL_REDUCEDT",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_ReduceDt },
	{ "UTIL_REFRACTORYADJUST",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_RefractoryAdjust },
	{ "UTIL_SAMPLE",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_Sample },
	{ "UTIL_SELECTCHANNELS",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_SelectChannels },
	{ "UTIL_SHAPEPULSE",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_ShapePulse },
	{ "UTIL_SIMSCRIPT",
		UTILITY_MODULE_CLASS, SIMSCRIPT_MODULE,
		InitModule_Utility_SimScript },
	{ "UTIL_STANDARDISE",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_Standardise },
	{ "UTIL_STROBE",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_Strobe },
	{ "UTIL_SWAPLR",
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_SwapLR },
	{ UTILITY_TRANSPOSE_MOD_NAME,
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_Transpose },

	{ "",
		0, MODULE_SPECIFIER_NULL,
		NULL }

	};

	return(&moduleList[index]);

}

/************************** UserList ******************************************/

/*
 * This routine returns an entry to the user module list.
 */

ModRegEntryPtr
UserList_ModuleReg(uShort index)
{
	if (!userModuleList)
		return(NULL);
	if (index > numUserModules)
		return(NULL);
	return(&userModuleList[index]);

}

/************************** Identify ******************************************/

/*
 * This function searches a list of registration entries.
 */

ModRegEntryPtr
Identify_ModuleReg(ModRegEntryPtr list, char *name)
{
	if (!list)
		return(NULL);
	for (  ; (StrNCmpNoCase_Utility_String(list->name, name) != 0) &&
	  (list->name[0] != '\0'); list++)
		;
	return ((list->name[0] == '\0')? NULL: list);

}

/************************** GetRegEntry ***************************************/

/*
 * This function returns a process module registration from the global lists.
 * It returns a pointer to the appropriate number, or zero if it does not find
 * identify the name.
 * All lists are expected to end with the null, "", string.
 * The names in the list are all assumed to be in upper case.
 * It ends terminally (exit(1)) if the list is not initialised.
 */

ModRegEntryPtr
GetRegEntry_ModuleReg(char *name)
{
	static const char *funcName = "GetRegEntry_ModuleReg";
	ModRegEntryPtr	regEntryPtr;

	if (name == NULL) {
		NotifyError("%s: Illegal null name.", funcName);
		return(NULL);
	}
	if (strlen(name) >= MAXLINE) {
		NotifyError("%s: Search name is too long (%s)", funcName, name);
		return(NULL);
	}
	if ((regEntryPtr = Identify_ModuleReg(LibraryList_ModuleReg(0), name)) !=
	  NULL)
		return(regEntryPtr);
	
	return(Identify_ModuleReg(userModuleList, name));

}

/************************** InitUserModuleList ********************************/

/*
 * This function initialises the user module list.
 * It returns FALSE if it fails in any way.
 */

BOOLN
InitUserModuleList_ModuleReg(int theMaxUserModules)
{
	static char *funcName = "InitUserModuleList_ModuleReg";
	int		i;
	ModRegEntryPtr	regEntry;

	if (userModuleList)
		free(userModuleList);
	maxUserModules = (theMaxUserModules > 0)? theMaxUserModules:
	  MODULE_REG_DEFAAULT_USER_MODULES;
	if ((userModuleList = (ModRegEntryPtr) calloc(maxUserModules + 1, sizeof(
	  ModRegEntry))) == NULL) {
		NotifyError("%s: Out of memory for %d entries.", funcName,
		  maxUserModules);
		return(FALSE);
	}
	regEntry = userModuleList;
	for (i = 0; i < maxUserModules + 1; i++, regEntry++) {
		regEntry->name = "";
		regEntry->specifier = PROCESS_MODULE;
		regEntry->InitModule = NULL;
	}
	numUserModules = 0;
	return(TRUE);

}

/************************** RegEntry ******************************************/

/*
 * This function registers a user module entry into the user module list.
 * it
 * It returns FALSE if it fails in any way.
 */

BOOLN
RegEntry_ModuleReg(const char *name, BOOLN (* InitModuleFunc)(ModulePtr))
{
	static const char *funcName = "RegEntry_ModuleReg";
	char	upperName[MAXLINE];
	ModRegEntryPtr	regEntry;

	if (numUserModules == maxUserModules) {
		NotifyError("%s: Maximum number (%d) of user modules registered.\n"
		  "Use the SetMaxUserModules_AppInterface to increase this number.",
		  funcName, maxUserModules);
		return(FALSE);
	}
	if (!name || (*name == '\0')) {
		NotifyError("%s: Illegal null or zero length module name.", funcName);
		return(FALSE);
	}
	if (GetRegEntry_ModuleReg((char *) name)) {
		NotifyError("%s: Module name '%s' already used.", funcName, name);
		return(FALSE);
	}
	if (!InitModuleFunc) {
		NotifyError("%s: InitModule function has not been set.", funcName);
		return(FALSE);
	}
	ToUpper_Utility_String(upperName, (char *) name);
	regEntry = &userModuleList[numUserModules++];
	regEntry->name = (char *) name;
	regEntry->classSpecifier = USER_MODULE_CLASS;
	regEntry->specifier = PROCESS_MODULE;
	regEntry->InitModule = InitModuleFunc;

	return(TRUE);

}

/************************** GetNumUserModules *********************************/

/*
 * This routine returns the number of user modules.
 */

int
GetNumUserModules_ModuleReg(void)
{
	/*static const char *funcName = "GetNumUserModules_ModuleReg";*/

	return(numUserModules);

}
