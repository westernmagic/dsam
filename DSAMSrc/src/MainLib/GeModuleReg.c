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
ModRegEntry	libraryModuleList[] = {

	{ "ANA_ACF", PROCESS_MODULE, InitModule_Analysis_ACF },
	{ "ANA_AVERAGES", PROCESS_MODULE, InitModule_Analysis_Averages },
	{ "ANA_CCF", PROCESS_MODULE, InitModule_Analysis_CCF },
	{ "ANA_CONVOLUTION", PROCESS_MODULE, InitModule_Analysis_Convolution },
	{ "ANA_FINDBIN", PROCESS_MODULE, InitModule_Analysis_FindBin },
	{ "ANA_FINDNEXTINDEX", PROCESS_MODULE, InitModule_Analysis_FindNextIndex },
	{ "ANA_FOURIERT", PROCESS_MODULE, InitModule_Analysis_FourierT },
	{ "ANA_HISTOGRAM", PROCESS_MODULE, InitModule_Analysis_Histogram },
	{ "ANA_INTENSITY", PROCESS_MODULE, InitModule_Analysis_Intensity },
	{ "ANA_ISIH", PROCESS_MODULE, InitModule_Analysis_ISIH },
	{ "ANA_SAI", ANA_SAI_MODULE, InitModule_Analysis_SAI },
	{ "ANA_SYNCHINDEX", PROCESS_MODULE, InitModule_Analysis_SynchIndex },
	{ "ANA_SPIKEREGULARITY", PROCESS_MODULE,
	  InitModule_Analysis_SpikeRegularity },
	{ "AN_SG_BINOMIAL", PROCESS_MODULE, InitModule_ANSpikeGen_Binomial },
	{ "AN_SG_CARNEY", PROCESS_MODULE, InitModule_ANSpikeGen_Carney },
	{ "AN_SG_SIMPLE", PROCESS_MODULE, InitModule_ANSpikeGen_Simple },
	{ "BM_CARNEY", PROCESS_MODULE, InitModule_BasilarM_Carney },
	{ "BM_COOKE", PROCESS_MODULE, InitModule_BasilarM_Cooke },
	{ "BM_GAMMAC", PROCESS_MODULE, InitModule_BasilarM_GammaChirp },
	{ "BM_GAMMAT", PROCESS_MODULE, InitModule_BasilarM_GammaT },
	{ "BM_DRNL", PROCESS_MODULE, InitModule_BasilarM_DRNL },
	{ "BM_DRNL_TEST", PROCESS_MODULE, InitModule_BasilarM_DRNL_Test },
	{ "DATAFILE_IN", PROCESS_MODULE, InitModule_DataFile },
	{ "DATAFILE_OUT", PROCESS_MODULE, InitModule_DataFile },
	{ "DISPLAY_SIGNAL", DISPLAY_MODULE, InitModule_SignalDisp },
	{ "FILT_BANDPASS", PROCESS_MODULE, InitModule_Filter_BandPass },
	{ "FILT_FIR", PROCESS_MODULE, InitModule_Filter_FIR },
	{ "FILT_LOWPASS", PROCESS_MODULE, InitModule_Filter_LowPass },
	{ "FILT_MULTIBPASS", PROCESS_MODULE, InitModule_Filter_MultiBPass },
	{ "IHC_CARNEY", PROCESS_MODULE, InitModule_IHC_Carney },
	{ "IHC_COOKE91", PROCESS_MODULE, InitModule_IHC_Cooke91 },
	{ "IHC_MEDDIS86", PROCESS_MODULE, InitModule_IHC_Meddis86 },
	{ "IHC_MEDDIS86A", PROCESS_MODULE, InitModule_IHC_Meddis86a },
	{ "IHC_MEDDIS2000", PROCESS_MODULE, InitModule_IHC_Meddis2000 },
	{ "IHCRP_CARNEY", PROCESS_MODULE, InitModule_IHCRP_Carney },
	{ "IHCRP_MEDDIS", PROCESS_MODULE, InitModule_IHCRP_Meddis },
	{ "IHCRP_SHAMMA", PROCESS_MODULE, InitModule_IHCRP_Shamma },
	{ "IHCRP_SHAMMA3StateVelIn", PROCESS_MODULE,
	  InitModule_IHCRP_Shamma3StateVelIn },
	{ "NEUR_ARLEKIM", PROCESS_MODULE, InitModule_Neuron_ArleKim },
	{ "NEUR_HHUXLEY", PROCESS_MODULE, InitModule_Neuron_HHuxley },
	{ "NEUR_MCGREGOR", PROCESS_MODULE, InitModule_Neuron_McGregor },
	{ "NULL", NULL_MODULE, SetNull_ModuleMgr },
#	ifdef _PAMASTER1_H
	{ "MPI_MASTER1", PROCESS_MODULE, InitModule_MPI_Master1 },
#	endif
	{ "STIM_CLICK", PROCESS_MODULE, InitModule_Click },
	{ "STIM_EXPGATEDTONE", PROCESS_MODULE, InitModule_Stimulus_ExpGatedTone },
	{ "STIM_HARMONIC", PROCESS_MODULE, InitModule_Harmonic },
	{ "STIM_PULSETRAIN", PROCESS_MODULE, InitModule_PulseTrain },
	{ "STIM_PURETONE", PROCESS_MODULE, InitModule_PureTone },
	{ "STIM_PURETONE_2", PROCESS_MODULE, InitModule_PureTone_2 },
	{ "STIM_PURETONE_AM", PROCESS_MODULE, InitModule_PureTone_AM },
	{ "STIM_PURETONE_BINAURAL", PROCESS_MODULE, InitModule_PureTone_Binaural },
	{ "STIM_PURETONE_FM", PROCESS_MODULE, InitModule_PureTone_FM },
	{ "STIM_PURETONE_MULTI", PROCESS_MODULE, InitModule_PureTone_Multi },
	{ "STIM_PURETONE_MULTIPULSE", PROCESS_MODULE,
	  InitModule_PureTone_MultiPulse },
	{ "STIM_STEPFUN", PROCESS_MODULE, InitModule_StepFunction },
	{ "STIM_WHITENOISE", PROCESS_MODULE, InitModule_WhiteNoise },
	{ "TRANS_GATE", PROCESS_MODULE, InitModule_Transform_Gate },
	{ "TRANS_SETDBSPL", PROCESS_MODULE, InitModule_Transform_SetDBSPL },
	{ "UTIL_ACCUMULATE", PROCESS_MODULE, InitModule_Utility_Accumulate },
	{ "UTIL_AMPMOD", PROCESS_MODULE, InitModule_Utility_AmpMod },
	{ "UTIL_BINSIGNAL", PROCESS_MODULE, InitModule_Utility_BinSignal },
	{ "UTIL_COMPRESSION", PROCESS_MODULE, InitModule_Utility_Compression },
	{ "UTIL_CONVMONAURAL", PROCESS_MODULE, InitModule_Utility_ConvMonaural },
	{ "UTIL_CREATEBINAURAL", PROCESS_MODULE, 
	  InitModule_Utility_CreateBinaural },
	{ "UTIL_CREATEBINAURALITD", PROCESS_MODULE,
	  InitModule_Utility_CreateBinauralITD },
	{ "UTIL_CREATEJOINED", PROCESS_MODULE, InitModule_Utility_CreateJoined },
	{ "UTIL_DELAY", PROCESS_MODULE, InitModule_Utility_Delay },
	{ "UTIL_HALFWAVERECTIFY", PROCESS_MODULE,
	  InitModule_Utility_HalfWaveRectify },
	{ "UTIL_ITERATEDRIPPLE", PROCESS_MODULE,
	  InitModule_Utility_IteratedRipple },
	{ "UTIL_PAUSE", PROCESS_MODULE, InitModule_Utility_Pause },
	{ "UTIL_REDUCECHANNELS", PROCESS_MODULE, InitModule_Utility_ReduceChannels
	  },
	{ "UTIL_REDUCEDT", PROCESS_MODULE, InitModule_Utility_ReduceDt },
	{ "UTIL_REFRACTORYADJUST", PROCESS_MODULE,
	  InitModule_Utility_RefractoryAdjust },
	{ "UTIL_SAMPLE", PROCESS_MODULE, InitModule_Utility_Sample },
	{ "UTIL_SELECTCHANNELS", PROCESS_MODULE,
	  InitModule_Utility_SelectChannels },
	{ "UTIL_SHAPEPULSE", PROCESS_MODULE, InitModule_Utility_ShapePulse },
	{ "UTIL_SIMSCRIPT", SIMSCRIPT_MODULE, InitModule_Utility_SimScript },
	{ "UTIL_STANDARDISE", PROCESS_MODULE, InitModule_Utility_Standardise },
	{ "UTIL_STROBE", PROCESS_MODULE, InitModule_Utility_Strobe },

	{ "", MODULE_SPECIFIER_NULL }

};

/******************************************************************************/
/************************** Subroutines and functions *************************/
/******************************************************************************/

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
	static char *funcName = "GetRegEntry_ModuleReg";
	ModRegEntryPtr	regEntryPtr;

	if (name == NULL) {
		NotifyError("%s: Illegal null name.", funcName);
		return(NULL);
	}
	if (strlen(name) >= MAXLINE) {
		NotifyError("%s: Search name is too long (%s)", funcName, name);
		return(NULL);
	}
	if ((regEntryPtr = Identify_ModuleReg(libraryModuleList, name)) != NULL)
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
	static char *funcName = "RegEntry_ModuleReg";
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
		NotifyError("%s: Module name '%s' already used.", funcName);
		return(FALSE);
	}
	if (!InitModuleFunc) {
		NotifyError("%s: InitModule function has not been set.", funcName);
		return(FALSE);
	}
	ToUpper_Utility_String(upperName, (char *) name);
	regEntry = &userModuleList[numUserModules++];
	regEntry->name = (char *) name;
	regEntry->specifier = PROCESS_MODULE;
	regEntry->InitModule = InitModuleFunc;

	return(TRUE);

}
