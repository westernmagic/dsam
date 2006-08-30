/**********************
 *
 * File:		UtIonChanList.c
 * Purpose:		This module generates the ion channel tables list, reading the
 *				tables from file or generating them as required.
 * Comments:	Written using ModuleProducer version 1.10 (Oct 13 1996).
 *				13-01-97: LPO: Introduce HHuxley Channel list mode.
 *				26-11-97 LPO: Using new GeNSpecLists module for 'mode'
 *				parameter.
 *				18-11-98 LPO: Introduced the 'SetParentICList_IonChanList' and
 *				the 'parentPtr' so that a ICList can always set the pointer
 *				which is pointing to it.
 *				08-12-98 LPO: Separated the IC channel generation routines into
 *				GetPars... and Generate... routines.
 *				14-12-98 LPO: Converted the 'InitTable_' so that it initialises
 *				the table for a single channel.  This means that it can be used
 *				by the 'ReadIonChannel_' routine as well as the others.
 *				03-02-99 LPO: Introduced the 'mode' field for the 'IonChannel'
 *				structure. This allows the 'SetICBaseMaxConducance' routine
 *				called from the GUI to set the correct maxConductance.  I also
 *				had to add the 'temperature' field too.
 *				Changed the 'InitTable_' routine to 'SetIonChannel'.
 * Author:		L. P. O'Mard
 * Created:		13 Oct 1996
 * Updated:		03 Feb 1999
 * Copyright:	(c) 1999, University of Essex.
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeNSpecLists.h"
#include "GeUniParMgr.h"
#include "FiParFile.h"
#include "UtNameSpecs.h"
#include "UtDynaList.h"
#include "UtString.h"
#include "UtIonChanList.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

NameSpecifier	iCListModeList[] = {
					
					{ wxT("BOLTZMANN"), ICLIST_BOLTZMANN_MODE },
					{ wxT("HHUXLEY"), 	ICLIST_HHUXLEY_MODE },
					{ wxT("FILE"), 		ICLIST_FILE_MODE },
					{ wxT(""),			ICLIST_NULL }
				};

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** FreeIonChannel ********************************/

/*
 * This function releases of the memory allocated for an ion channels members.
 * The structure itself was not allocated, so it does not require deallocation.
 */

void
FreeIonChannel_IonChanList(IonChannelPtr *theIC)
{
	if (!*theIC)
		return;
	free((*theIC)->table);
	FreeList_UniParMgr(&(*theIC)->parList);
	free(*theIC);
	*theIC = NULL;

}

/****************************** FreeIonChannels *******************************/

/*
 * This function releases the memory allocated for all ion channels.
 */

void
FreeIonChannels_IonChanList(IonChanListPtr theICList)
{
	IonChannelPtr	theIC;

	if (!theICList)
		return;
	while (theICList->ionChannels) {
		theIC = (IonChannelPtr) Pull_Utility_DynaList(&theICList->ionChannels);
			FreeIonChannel_IonChanList(&theIC);
	}

}

/****************************** Free ******************************************/

/*
 * This function releases the memory allocated for the ion channel list.
 * It is defined as returning a BOOLN value because the generic
 * module interface requires that a non-void value be returned.
 */

BOOLN
Free_IonChanList(IonChanListPtr *theICList)
{
	if (*theICList == NULL)
		return(FALSE);
	FreeIonChannels_IonChanList(*theICList);
	if ((*theICList)->printTablesModeList)
		free((*theICList)->printTablesModeList);
	FreeList_UniParMgr(&(*theICList)->parList);
	free(*theICList);
	*theICList = NULL;
	return(TRUE);

}

/****************************** Init ******************************************/

/*
 * This function allocates memory for the ion channel list structure.
 * The function returns a pointer to the prepared structure.
 */

IonChanListPtr
Init_IonChanList(const WChar *callingFunctionName)
{
	static const WChar	*funcName = wxT("Init_IonChanList");
	IonChanListPtr	theICList;

	if ((theICList = (IonChanListPtr) malloc(sizeof(IonChanList))) == NULL) {
		NotifyError(wxT("%s: Out of memory (called by %s)."), funcName,
		  callingFunctionName);
		return(NULL);
	}
	theICList->updateFlag = TRUE;
	theICList->useTemperatureCalcFlag = TRUE;
	theICList->numChannels = 0;
	theICList->ionChannels = NULL;
	theICList->printTablesMode = 0;
	theICList->minVoltage = 0.0;
	theICList->maxVoltage = 0.0;
	theICList->dV = 0.0;
	DSAM_strcpy(theICList->diagFileName, DEFAULT_FILE_NAME);

	if ((theICList->printTablesModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), theICList->diagFileName)) == NULL) {
	  	free(theICList);
		return(NULL);
	}
	theICList->parList = NULL;
	theICList->currentIC = NULL;
	theICList->oldNumChannels = 0;
	theICList->oldNumTableEntries = 0;
	return(theICList);

}

/****************************** InitICHHuxleyPars *****************************/

/*
 * This routine initialises the HHuxley parameters.
 * This is a structural initialisation for setting up the pointers to the
 * gate arrays, so that they can be passed to the universal parameter list
 */

BOOLN
InitICHHuxleyPars_IonChanList(ICHHuxleyParsPtr p)
{
	static const WChar	*funcName = wxT("InitHHuxleyPars_IonChanList");

	if (p == NULL) {
		NotifyError(wxT("%s: Pointer not initialised."), funcName);
		return(FALSE);
	}
	SET_IC_GATE_ARRAY(p->aA); SET_IC_GATE_ARRAY(p->aB);
	SET_IC_GATE_ARRAY(p->aC); SET_IC_GATE_ARRAY(p->aD);
	SET_IC_GATE_ARRAY(p->aE); SET_IC_GATE_ARRAY(p->aF);
	SET_IC_GATE_ARRAY(p->aG); SET_IC_GATE_ARRAY(p->aH);
	SET_IC_GATE_ARRAY(p->aI); SET_IC_GATE_ARRAY(p->aJ);
	SET_IC_GATE_ARRAY(p->aK); SET_IC_GATE_ARRAY(p->bA);
	SET_IC_GATE_ARRAY(p->bB); SET_IC_GATE_ARRAY(p->bC);
	SET_IC_GATE_ARRAY(p->bD); SET_IC_GATE_ARRAY(p->bE);
	SET_IC_GATE_ARRAY(p->bF); SET_IC_GATE_ARRAY(p->bG);
	SET_IC_GATE_ARRAY(p->bH); SET_IC_GATE_ARRAY(p->bI);
	SET_IC_GATE_ARRAY(p->bJ); SET_IC_GATE_ARRAY(p->bK);
	return(TRUE);

}

/****************************** InitICBoltzmannPars ***************************/

/*
 * This routine initialises the Boltzmann parameters.
 * This is a structural initialisation for setting up the pointers to the
 * gate arrays, so that they can be passed to the universal parameter list
 */

BOOLN
InitICBoltzmannPars_IonChanList(ICBoltzmannParsPtr p)
{
	static const WChar	*funcName = wxT("InitICBoltzmannPars_IonChanList");

	if (p == NULL) {
		NotifyError(wxT("%s: Pointer not initialised."), funcName);
		return(FALSE);
	}
	SET_IC_GATE_ARRAY(p->halfMaxV);
	SET_IC_GATE_ARRAY(p->zZ);
	SET_IC_GATE_ARRAY(p->tau);
	return(TRUE);

}

/****************************** InitIonChannel ********************************/

/*
 * This function allocates memory for the ion channel structure.
 * The function returns a pointer to the prepared structure.
 * The 'calloc' function is used so that all the values are set to zero.
 */

IonChannelPtr
InitIonChannel_IonChanList(const WChar *callingFunctionName,
  int numTableEntries)
{
	static const WChar	*funcName = wxT("InitIonChannel_IonChanList");
	IonChannelPtr	theIC;

	if ((theIC = (IonChannelPtr) calloc(1, sizeof(IonChannel))) == NULL) {
		NotifyError(wxT("%s: Out of memory (called by %s)."), funcName,
		  callingFunctionName);
		return(NULL);
	}
	theIC->updateFlag = TRUE;
	theIC->mode = ICLIST_NULL;
	theIC->enabled = GENERAL_BOOLEAN_ON;
	theIC->description[0] = '\0';
	theIC->numTableEntries = numTableEntries;
	theIC->activationExponent = 0.0;
	theIC->equilibriumPot = 0.0;
	theIC->temperature = 0.0;
	theIC->baseMaxConductance = 0.0;
	theIC->maxConductance = 0.0;
	theIC->conductanceQ10 = 0.0;
	theIC->minVoltage = 0.0;
	theIC->maxVoltage = 0.0;
	theIC->dV = 0.0;
	InitICHHuxleyPars_IonChanList(&theIC->hHuxley);
	InitICBoltzmannPars_IonChanList(&theIC->boltzmann);
	DSAM_strcpy(theIC->fileName, DEFAULT_FILE_NAME);
	theIC->PowFunc = pow;
	theIC->parList = NULL;
	if ((theIC->table = (ICTableEntry *) calloc(theIC->numTableEntries,
	  sizeof(ICTableEntry))) == NULL) {
		NotifyError(wxT("%s: Out of memory for table entries (%d)."), funcName,
		 theIC->numTableEntries);
		FreeIonChannel_IonChanList(&theIC);
		return(NULL);
	}
	return(theIC);

}

/********************************* SetGeneralUniParListMode *******************/

/*
 * This routine enables and disables the respective parameters for each ICList
 * mode.
 * It assumes that the parameter list has been correctly initialised.
 */
 
BOOLN
SetGeneralUniParListMode_CFList(IonChanListPtr theICs)
{
	static const WChar *funcName = wxT("SetGeneralUniParListMode_CFList");
	int		i;

	if (!CheckInit_IonChanList(theICs, funcName))
		return(FALSE);
	for (i = ICLIST_NUM_CONSTANT_PARS; i < CFLIST_NUM_PARS; i++)
		theICs->parList->pars[i].enabled = FALSE;
	if (theICs->useTemperatureCalcFlag) {
		theICs->parList->pars[ICLIST_TEMPERATURE].enabled = TRUE;
		theICs->parList->pars[ICLIST_LEAKAGE_COND_Q10].enabled = TRUE;
	}
	return(TRUE);

}

/********************************* SetGeneralUniParList ***********************/

/*
 * This routine initialises and sets the IonChanList's universal parameter list.
 * This list provides universal access to the IonChanList's parameters.
 */
 
BOOLN
SetGeneralUniParList_IonChanList(IonChanListPtr theICs)
{
	static const WChar *funcName = wxT("SetGeneralUniParList_IonChanList");
	UniParPtr	pars;

	if (!CheckInit_IonChanList(theICs, funcName))
		return(FALSE);
	FreeList_UniParMgr(&theICs->parList);
	if ((theICs->parList = InitList_UniParMgr(UNIPAR_SET_ICLIST,
	  ICLIST_NUM_PARS, theICs)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = theICs->parList->pars;
	SetPar_UniParMgr(&pars[ICLIST_PRINTTABLESMODE], wxT("TABLES_MODE"),
	 wxT("Print ion channel tables mode ('off', 'screen' or filename)."),
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &theICs->printTablesMode, theICs->printTablesModeList,
	  (void * (*)) SetPrintTablesMode_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_BASELEAKAGECOND], wxT("LEAKAGE_COND"),
	 wxT("Base leakage conductance (S)."),
	  UNIPAR_REAL,
	  &theICs->baseLeakageCond, NULL,
	  (void * (*)) SetBaseLeakageCond_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_LEAKAGEPOT], wxT("LEAKAGE_POT"),
	 wxT("Leakage equilibrium potential (V)."),
	  UNIPAR_REAL,
	  &theICs->leakagePot, NULL,
	  (void * (*)) SetLeakagePot_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_NUMCHANNELS], wxT("NUM_CHANNELS"),
	 wxT("Number of ion channels."),
	  UNIPAR_INT,
	  &theICs->numChannels, NULL,
	  (void * (*)) SetNumChannels_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_MIN_VOLT], wxT("MIN_VOLT"),
	 wxT("Minimum voltage for tables (V)."),
	  UNIPAR_REAL,
	  &theICs->minVoltage, NULL,
	  (void * (*)) SetMinVoltage_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_MAX_VOLT], wxT("MAX_VOLT"),
	 wxT("Maximum voltage for tables (V)."),
	  UNIPAR_REAL,
	  &theICs->maxVoltage, NULL,
	  (void * (*)) SetMaxVoltage_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_DV], wxT("DV"),
	 wxT("Voltage step for tables (V)."),
	  UNIPAR_REAL,
	  &theICs->dV, NULL,
	  (void * (*)) SetVoltageStep_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_TEMPERATURE], wxT("TEMPERATURE"),
	 wxT("Operating temperature (degrees C)."),
	  UNIPAR_REAL,
	  &theICs->temperature, NULL,
	  (void * (*)) SetTemperature_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_LEAKAGE_COND_Q10], wxT("LEAKAGE_C_Q10)"),
	 wxT("Leakage conductance Q10."),
	  UNIPAR_REAL,
	  &theICs->leakageCondQ10, NULL,
	  (void * (*)) SetLeakageCondQ10_IonChanList);
	return(TRUE);

}

/********************************* SetIonChannelUniParListMode ****************/

/*
 * This routine enables and disables the respective parameters for each
 * Ion channel mode.
 * It assumes that the parameter list has been correctly initialised.
 */
 
BOOLN
SetIonChannelUniParListMode_IonChanList(IonChannelPtr theIC)
{
	static const WChar *funcName = wxT(
	  "SetIonChannelUniParListMode_IonChanList");
	int		i;

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	for (i = ICLIST_IC_NUM_CONSTANT_PARS; i < ICLIST_IC_NUM_PARS; i++)
		theIC->parList->pars[i].enabled = FALSE;
	switch (theIC->mode) {
	case ICLIST_BOLTZMANN_MODE:
		theIC->parList->pars[ICLIST_IC_CONDUCTANCE_Q10].enabled = TRUE;
		theIC->parList->pars[ICLIST_IC_V_HALF].enabled = TRUE;
		theIC->parList->pars[ICLIST_IC_Z].enabled = TRUE;
		theIC->parList->pars[ICLIST_IC_TAU].enabled = TRUE;
		break;
	case ICLIST_HHUXLEY_MODE:
		theIC->parList->pars[ICLIST_IC_CONDUCTANCE_Q10].enabled = TRUE;
		for (i = ICLIST_IC_ALPHA_A; i <= ICLIST_IC_BETA_K; i++)
			theIC->parList->pars[i].enabled = TRUE;
		break;
	case ICLIST_FILE_MODE:
		theIC->parList->pars[ICLIST_IC_FILE_NAME].enabled = TRUE;
		break;
	default:
		NotifyError(wxT("%s: Unknown ion channel mode (%d).\n"), funcName,
		  theIC->mode);
		return(FALSE);
	}
	return(TRUE);

}

/********************************* SetIonChannelUniParList ********************/

/*
 * This routine initialises and sets the IonChannel's universal parameter list.
 * This list provides universal access to the IonChanList's parameters.
 */

BOOLN
SetIonChannelUniParList_IonChanList(IonChanListPtr theICs, IonChannelPtr theIC)
{
	static const WChar *funcName = wxT("SetIonChannelUniParList_IonChanList");
	static int numGates = ICLIST_NUM_GATES;
	UniParPtr	pars;

	if (!CheckInitIC_IonChanList(theIC, funcName))
		return(FALSE);
	FreeList_UniParMgr(&theIC->parList);
	if ((theIC->parList = InitList_UniParMgr(UNIPAR_SET_IC, ICLIST_IC_NUM_PARS,
	  theICs)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = theIC->parList->pars;
	SetPar_UniParMgr(&pars[ICLIST_IC_DESCRIPTION], wxT("DESCRIPTION"),
	 wxT("Description."),
	  UNIPAR_STRING,
	  &theIC->description, NULL,
	  (void * (*)) SetICDescription_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_MODE], wxT("MODE"),
	 wxT("Mode option ('file', 'hHuxley' or 'boltzmann')."),
	  UNIPAR_NAME_SPEC,
	  &theIC->mode, iCListModeList,
	  (void * (*)) SetICMode_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_ENABLED], wxT("ENABLED"),
	 wxT("Ion channel enabled status ('on' or 'off)."),
	  UNIPAR_BOOL,
	  &theIC->enabled, NULL,
	  (void * (*)) SetICEnabled_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_EQUILIBRIUM_POT], wxT("EQUIL_POT"),
	 wxT("Equilibrium potential (V)."),
	  UNIPAR_REAL,
	  &theIC->equilibriumPot, NULL,
	  (void * (*)) SetICEquilibriumPot_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_CONDUCTANCE], wxT("BASE_MAX_COND"),
	 wxT("Base maximum conductance (S)."),
	  UNIPAR_REAL,
	  &theIC->baseMaxConductance, NULL,
	  (void * (*)) SetICBaseMaxConductance_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_ACTIVATION_EXPONENT], wxT("ACTIVATION"),
	 wxT("Activation exponent (real)."),
	  UNIPAR_REAL,
	  &theIC->activationExponent, NULL,
	  (void * (*)) SetICActivationExponent_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_CONDUCTANCE_Q10], wxT("COND_Q10"),
	 wxT("Conductance Q10."),
	  UNIPAR_REAL,
	  &theIC->conductanceQ10, NULL,
	  (void * (*)) SetICConductanceQ10_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_V_HALF], wxT("V_HALF"),
	 wxT("Voltage at half maximum values (V)"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->boltzmann.halfMaxV.ptr, &numGates,
	  (void * (*)) SetICBoltzmannHalfMaxV_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_Z], wxT("Z"),
	 wxT("'Z' Constant values (unit)."),
	  UNIPAR_REAL_ARRAY,
	  &theIC->boltzmann.zZ.ptr, &numGates,
	  (void * (*)) SetICBoltzmannZ_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_TAU], wxT("TAU"),
	 wxT("Time constants, tau (s)"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->boltzmann.tau.ptr, &numGates,
	  (void * (*)) SetICBoltzmannTau_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FILE_NAME], wxT("FILENAME"),
	 wxT("Ion channel file name."),
	  UNIPAR_FILE_NAME,
	  &theIC->fileName, (WChar *) wxT("*.par"),
	  (void * (*)) SetICFileName_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_ALPHA_A], wxT("ALPHA_A"),
	 wxT("Alpha 'a' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.aA.ptr, &numGates,
	  (void * (*)) SetICHHuxleyAlphaA_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_ALPHA_B], wxT("ALPHA_B"),
	 wxT("Alpha 'b' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.aB.ptr, &numGates,
	  (void * (*)) SetICHHuxleyAlphaB_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_ALPHA_C], wxT("ALPHA_C"),
	 wxT("Alpha 'c' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.aC.ptr, &numGates,
	  (void * (*)) SetICHHuxleyAlphaC_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_ALPHA_D], wxT("ALPHA_D"),
	 wxT("Alpha 'd' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.aD.ptr, &numGates,
	  (void * (*)) SetICHHuxleyAlphaD_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_ALPHA_E], wxT("ALPHA_E"),
	 wxT("Alpha 'e' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.aE.ptr, &numGates,
	  (void * (*)) SetICHHuxleyAlphaE_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_ALPHA_F], wxT("ALPHA_F"),
	 wxT("Alpha 'f' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.aF.ptr, &numGates,
	  (void * (*)) SetICHHuxleyAlphaF_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_ALPHA_G], wxT("ALPHA_G"),
	 wxT("Alpha 'g' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.aG.ptr, &numGates,
	  (void * (*)) SetICHHuxleyAlphaG_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_ALPHA_H], wxT("ALPHA_H"),
	 wxT("Alpha 'h' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.aH.ptr, &numGates,
	  (void * (*)) SetICHHuxleyAlphaH_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_ALPHA_I], wxT("ALPHA_I"),
	 wxT("Alpha 'i' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.aI.ptr, &numGates,
	  (void * (*)) SetICHHuxleyAlphaI_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_ALPHA_J], wxT("ALPHA_J"),
	 wxT("Alpha 'j' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.aJ.ptr, &numGates,
	  (void * (*)) SetICHHuxleyAlphaJ_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_ALPHA_K], wxT("ALPHA_K"),
	 wxT("Alpha 'k' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.aK.ptr, &numGates,
	  (void * (*)) SetICHHuxleyAlphaK_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_BETA_A], wxT("BETA_A"),
	 wxT("Beta 'a' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.bA.ptr, &numGates,
	  (void * (*)) SetICHHuxleyBetaA_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_BETA_B], wxT("BETA_B"),
	 wxT("Beta 'b' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.bB.ptr, &numGates,
	  (void * (*)) SetICHHuxleyBetaB_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_BETA_C], wxT("BETA_C"),
	 wxT("Beta 'c' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.bC.ptr, &numGates,
	  (void * (*)) SetICHHuxleyBetaC_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_BETA_D], wxT("BETA_D"),
	 wxT("Beta 'd' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.bD.ptr, &numGates,
	  (void * (*)) SetICHHuxleyBetaD_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_BETA_E], wxT("BETA_E"),
	 wxT("Beta 'e' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.bE.ptr, &numGates,
	  (void * (*)) SetICHHuxleyBetaE_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_BETA_F], wxT("BETA_F"),
	 wxT("Beta 'f' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.bF.ptr, &numGates,
	  (void * (*)) SetICHHuxleyBetaF_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_BETA_G], wxT("BETA_G"),
	 wxT("Beta 'g' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.bG.ptr, &numGates,
	  (void * (*)) SetICHHuxleyBetaG_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_BETA_H], wxT("BETA_H"),
	 wxT("Beta 'h' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.bH.ptr, &numGates,
	  (void * (*)) SetICHHuxleyBetaH_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_BETA_I], wxT("BETA_I"),
	 wxT("Beta 'i' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.bI.ptr, &numGates,
	  (void * (*)) SetICHHuxleyBetaI_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_BETA_J], wxT("BETA_J"),
	 wxT("Beta 'j' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.bJ.ptr, &numGates,
	  (void * (*)) SetICHHuxleyBetaJ_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_BETA_K], wxT("BETA_K"),
	 wxT("Beta 'k' parameter values for activation and inactivation"),
	  UNIPAR_REAL_ARRAY,
	  &theIC->hHuxley.bK.ptr, &numGates,
	  (void * (*)) SetICHHuxleyBetaK_IonChanList);
	return(TRUE);

}

/****************************** CheckPars *************************************/

/*
 * This routine checks that the necessary parameters for a ion channel list
 *.have been correctly initialised.
 * It returns TRUE if there are no problems.
 */

BOOLN
CheckPars_IonChanList(IonChanListPtr theICList)
{
	static const WChar	*funcName = wxT("CheckPars_IonChanList");
	BOOLN	ok;

	ok = TRUE;
	if (!CheckInit_IonChanList(theICList, funcName))
		return(FALSE);
	if (theICList->ionChannels == NULL) {
		NotifyError(wxT("%s: ionChannels array not set."), funcName);
		ok = FALSE;
	}
	if (!theICList->numChannels <= 0) {
		NotifyError(wxT("%s: Number of Channels not correctly set."), funcName);
		ok = FALSE;
	}
	return(ok);

}

/****************************** PrintIonChannelPars ***************************/

/*
 * This routine prints all the ion channel parameters.
 * It assumes that the structure has been correctly initialised.
 */

void
PrintIonChannelPars_IonChanList(IonChannelPtr theIC)
{
	static const WChar *funcName = wxT("PrintIonChannelPars_IonChanList");
	int		i;

	if (!CheckInitIC_IonChanList(theIC, funcName))
		return;
	DPrint(wxT("\t\t<---- Ion channel %s mode: %s ---->\n"), iCListModeList[
	  theIC->mode].name, theIC->description);
	DPrint(wxT("\t\tEanbled status: %s,"), BooleanList_NSpecLists(
	  theIC->enabled)->name);
	DPrint(wxT("\tEquilibrium Potential: %g (mV),\n"),
	  MILLI(theIC->equilibriumPot));
	switch (theIC->mode) {
	case ICLIST_BOLTZMANN_MODE: {
		ICBoltzmannParsPtr	p = &theIC->boltzmann;
		DPrint(wxT("\t\tMax. conductance: %.3g S,"), theIC->maxConductance);
		DPrint(wxT("\tConductance Q10: %g,\n"), theIC->conductanceQ10);
		DPrint(wxT("\t\t\t%s\t%s\t%s\n"), wxT("V_1/2 (mV)"), wxT("Z (units)"),
		  wxT("tau (ms)"));
		for (i = 0; i < ICLIST_NUM_GATES; i++)
			DPrint(wxT("\t\t\t%g\t\t%g\t\t%g\n"), MILLI(p->halfMaxV.array[i]),
			  p->zZ.array[i], MILLI(p->tau.array[i]));
		break; }
	case ICLIST_HHUXLEY_MODE: {
		ICHHuxleyParsPtr	p = &theIC->hHuxley;
		DPrint(wxT("\t\tBase max. conductance: %.3g (%.3g @ %g oC.) ")
		  wxT("(S),\n"), theIC->baseMaxConductance, theIC->maxConductance,
		  theIC->temperature);
		DPrint(wxT("\t\tConductance Q10: %g,\n"), theIC->conductanceQ10);
		DPrint(wxT("\t\t%6s%5s%5s%6s%5s%5s%6s%3s%3s%3s%3s\n"), wxT("a"), wxT(
		  "b"), wxT("c"), wxT("d"), wxT("e"), wxT("f"), wxT("g"), wxT("h"), wxT(
		  "i"), wxT("j"), wxT("k"));
		for (i = 0; i < ICLIST_NUM_GATES; i++) {
			DPrint(wxT("\t\t%6g%5g%5g%6g%5g%5g%6g%3g%3g%3g%3g Alpha\n"),
			  p->aA.array[i], p->aB.array[i], p->aC.array[i], p->aD.array[i],
			  p->aE.array[i], p->aF.array[i], p->aG.array[i], p->aH.array[i],
			  p->aI.array[i], p->aJ.array[i], p->aK.array[i]);
			DPrint(wxT("\t\t%6g%5g%5g%6g%5g%5g%6g%3g%3g%3g%3g Beta\n"),
			  p->bA.array[i], p->bB.array[i], p->bC.array[i], p->bD.array[i],
			  p->bE.array[i], p->bF.array[i], p->bG.array[i], p->bH.array[i],
			  p->bI.array[i], p->bJ.array[i], p->bK.array[i]);
		}
		break; }
	case ICLIST_FILE_MODE:
		DPrint(wxT("\t\tIonchannel table file: %s,\n"), theIC->fileName);
		DPrint(wxT("\t\tMax. conductance: %.3g S,\n"), theIC->maxConductance);
		break;
	default:
		;
	}
	DPrint(wxT("\t\tActivation exponent: %g,"), theIC->activationExponent);
	DPrint(wxT("\tNo. of table entries: %d\n"), theIC->numTableEntries);

}

/****************************** PrintTables ***********************************/

/*
 * This routine prints all the ion channel tables and parameters.
 * It assumes that the structure has been correctly initialised.
 */

void
PrintTables_IonChanList(IonChanListPtr theICList)
{
	static const WChar *funcName = wxT("PrintTables_IonChanList");
	int		i;
	FILE	*fp = NULL, *savedFp;
	DynaListPtr	node;
	ICTableEntryPtr	e;
	IonChannelPtr theIC;

	if (!CheckInit_IonChanList(theICList, funcName))
		return;
	OpenDiagnostics_NSpecLists(&fp, theICList->printTablesModeList,
	  theICList->printTablesMode);
	savedFp = GetDSAMPtr_Common()->parsFile;
	GetDSAMPtr_Common()->parsFile = fp;
	for (node = theICList->ionChannels; node != NULL; node = node->next) {
		theIC = (IonChannelPtr) node->data;
		PrintIonChannelPars_IonChanList(theIC);
		DPrint(wxT("\t\t\t%7s\t%7s\t%7s\t%7s\t%7s\n"), wxT("   V   "), wxT(
		  "   Y   "), wxT(" tau_y "), wxT("   Z   "), wxT(" tau_z "));
		DPrint(wxT("\t\t\t%7s\t%7s\t%7s\t%7s\t%7s\n"), wxT("  (mV) "), wxT(
		  " (0-1) "), wxT("  (ms) "), wxT(" (0-1) "), wxT("  (ms) "));
		for (i = 0; i < theIC->numTableEntries; i++) {
			e = &theIC->table[i];
			DPrint(wxT("\t\t\t%7g\t%7.4f\t%7.4g\t%7.4f\t%7.4g\n"),
			  MILLI(theIC->minVoltage + i * theIC->dV), e->yY, MSEC(e->ty),
			  e->zZ, MSEC(e->tz));
		}
	}
	GetDSAMPtr_Common()->parsFile = savedFp;
	CloseDiagnostics_NSpecLists(&fp);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the ion channel list parameters.
 */

BOOLN
PrintPars_IonChanList(IonChanListPtr theICs)
{
	static const WChar *funcName = wxT("PrintPars_IonChanList");
	DynaListPtr	node;

	if (!CheckInit_IonChanList(theICs, funcName)) {
		NotifyError(wxT("%s: Ion channel list parameters not been correctly ")
		  wxT("set."), funcName);
		return(FALSE);
	}
	DPrint(wxT("\t\tIon channel list structure parameters:-\n"));
	DPrint(wxT("\t\tPrint tables mode: %s,\n"), theICs->printTablesModeList[
	  theICs->printTablesMode].name);
	if (theICs->useTemperatureCalcFlag) {
		DPrint(wxT("\t\tBase leakage conductance = %.3g (%.3g @ %g oC.) (S),")
		  wxT("\n"), theICs->baseLeakageCond, theICs->leakageCond, theICs->
		  temperature);
		DPrint(wxT("\t\tLeakage conductance Q10 = %g,\n"), theICs->
		  leakageCondQ10);
	} else
		DPrint(wxT("\t\tLeakage conductance = %.3g S,\n"), theICs->leakageCond);
	DPrint(wxT("\t\tLeakage potential = %g (mV),\n"), MILLI(theICs->
	  leakagePot));
	DPrint(wxT("\t\tNo. ion channels = %d\n"), theICs->numChannels);
	DPrint(wxT("\t\tMin./max voltage: %g / %g (mV),"), MILLI(theICs->
	  minVoltage), MILLI(theICs->maxVoltage));
	DPrint(wxT("\tVoltage step: %g (mV),\n"), MILLI(theICs->dV));
	if (theICs->printTablesMode)
		PrintTables_IonChanList(theICs);
	else
		for (node = theICs->ionChannels; node != NULL; node = node->next)
			PrintIonChannelPars_IonChanList((IonChannelPtr) node->data);
	return(TRUE);

}

/****************************** ReadVoltageTable ******************************/

/*
 * This function reads an Ion channel format file.
 * It returns a pointer to the data structure, or NULL if it fails in any way.
 */

BOOLN
ReadVoltageTable_IonChanList(IonChannelPtr theIC,  FILE *fp)
{
	static const WChar	*funcName = wxT("ReadVoltageTable_IonChanList");
	BOOLN	ok = TRUE;
	int		i;
	double	v, dV;

	for (i = 0; ok && (i < theIC->numTableEntries); i++) {
		ok = GetPars_ParFile(fp, wxT("%lf %lf %lf %lf %lf"), &v, &theIC->table[
		  i].yY, &theIC->table[i].ty, &theIC->table[i].zZ, &theIC->table[i].tz);
		if (ok && ((fabs(theIC->table[i].ty) < DBL_EPSILON) ||
		  (fabs(theIC->table[i].ty) < DBL_EPSILON))) {
			NotifyError(wxT("%s: The tau values must be greater than zero ")
			  wxT("(entry %d)."), funcName, i);
			ok = FALSE;
		}
		if (ok)
			switch (i) {
			case 0:
				if (fabs(theIC->minVoltage - v) > DBL_EPSILON) {
					NotifyError(wxT("%s: Incorrect minimum voltage for cell ")
					  wxT("(%g mV)."), funcName, MILLI(v));
					ok = FALSE;
				}
				break;
			case 1:
				dV = v - theIC->minVoltage;
				if (fabs(theIC->dV - dV) > DBL_EPSILON) {
					NotifyError(wxT("%s: Incorrect voltage step for cell (%g ")
					  wxT("mV)."), funcName, MILLI(dV));
					ok = FALSE;
				}
				break;
			default:
				if (fabs(theIC->minVoltage + i * theIC->dV - v) > DBL_EPSILON) {
					NotifyError(wxT("%s: Table entry voltage out of sequence: ")
					  wxT("entry %d = %g mV."), funcName, i, MILLI(v));
					ok = FALSE;
				}
			}
	}
	fclose(fp);
	if (!ok)
		NotifyError(wxT("%s: Failed to read table: insufficient entries for\n")
		  wxT("voltage range specification."), funcName);
	return(ok);

}
	
/****************************** HHuxleyAlpha **********************************/

/*
 * This function calculates the alpha parameter for the HHuxley et al. tables.
 * It returns 1.0e3 if all the unified equation parameters are zero, this will
 * mean the activation and activation time constants will be 1.0.
 * It assumes that alpha is continuous about any singularities that occur in
 * the equation, i.e. mV = -f or mV = -k.
 */


double
HHuxleyAlpha_IonChanList(double a, double b, double c, double d, double e,
  double f, double g, double h, double i, double j, double k, double mV,
  double mDV, double temperature)
{
	double	previous, next;

	if (HHUXLEY_ALL_ZERO_PARS(a, b, c, d, e, f, g, h, i, j, k))
		return(1.0e-3);
	if ((fabs(mV + f) > DBL_EPSILON) && (fabs(mV + k) > DBL_EPSILON))
		return(a * HHUXLEY_TF(b, temperature) * (c * mV + d) / (1 + e *
		  exp((mV + f) / g)) + h * HHUXLEY_TF(i, temperature) / (1 + j *
		  exp(mV + k)));
	else {
		previous = HHuxleyAlpha_IonChanList(a, b, c, d, e, f, g, h, i, j, k,
		  mV - mDV, mDV, temperature);
		next = HHuxleyAlpha_IonChanList(a, b, c, d, e, f, g, h, i, j, k,
		  mV + mDV, mDV, temperature);
		return((previous + next) / 2.0);
	}
}


/****************************** HHuxleyBeta ***********************************/

/*
 * This function calculates the beta parameter for the HHuxley et al. tables.
 * It returns 0.0 if all the unified equation parameters are zero, this will
 * mean the activation and activation time constants will be 1.0.
 * It assumes that beta is continuous about any singularities that occur in
 * the equation, i.e. mV = -j.
 */


double
HHuxleyBeta_IonChanList(double a, double b, double c, double d, double e,
  double f, double g, double h, double i, double j, double k, double mV,
  double mDV, double temperature)
{
	double	previous, next;

	if (HHUXLEY_ALL_ZERO_PARS(a, b, c, d, e, f, g, h, i, j, k))
		return(0.0);
	if (fabs(mV + j) > DBL_EPSILON)
		return(a * HHUXLEY_TF(b, temperature) * exp((mV + c) / d) + e *
		  HHUXLEY_TF(f, temperature) * (g * mV + h) / (1 + i * exp((mV + j) /
		  k)));
	else {
		previous = HHuxleyBeta_IonChanList(a, b, c, d, e, f, g, h, i, j, k,
		  mV - mDV, mDV, temperature);
		next = HHuxleyBeta_IonChanList(a, b, c, d, e, f, g, h, i, j, k,
		  mV + mDV, mDV, temperature);
		return((previous + next) / 2.0);
	}
}

/****************************** GetParsHHuxley ********************************/

/*
 * This function reads the parameters for the Boltzmann ion channels.
 * It expects the data structure to be passed to the routine as an argument.
 * The function returns TRUE if it succeeds, otherwise it returns FALSE.
 */
 
BOOLN
GetParsHHuxley_IonChanList(IonChannelPtr theIC, FILE *fp)
{
	static const WChar	*funcName = wxT("GetParsHHuxley_IonChanList");
	BOOLN	ok = TRUE;
	int		j;
	ICHHuxleyParsPtr	p;

	if (!GetPars_ParFile(fp, wxT("%lf"), &theIC->conductanceQ10))
		ok = FALSE;
	p = &theIC->hHuxley;
	for (j = 0; j < ICLIST_NUM_GATES; j++) {
		if (!GetPars_ParFile(fp, wxT("%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf ")
		  wxT("%lf"), &p->aA.array[j], &p->aB.array[j], &p->aC.array[j],
		  &p->aD.array[j], &p->aE.array[j], &p->aF.array[j],
		  &p->aG.array[j], &p->aH.array[j], &p->aI.array[j],
		  &p->aJ.array[j], &p->aK.array[j]))
			ok = FALSE;
		if (!GetPars_ParFile(fp, wxT("%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf ")
		  wxT("%lf"),  &p->bA.array[j], &p->bB.array[j], &p->bC.array[j],
		  &p->bD.array[j], &p->bE.array[j], &p->bF.array[j],
		  &p->bG.array[j], &p->bH.array[j], &p->bI.array[j],
		  &p->bJ.array[j], &p->bK.array[j]))
			ok = FALSE;
	}

	if (!ok) {
		NotifyError(wxT("%s: Could not read parameters for channel '%s'."),
		  funcName, theIC->description);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** GenerateHHuxley *******************************/

/*
 * This function generates a list of ion channels in an ICList data structure,
 * using the HHuxley et al. equations (Rothman J. S. , Young E. D. and Manis P.
 * B. (1993) " Convergence of Auditory Nerve Fibres Onto Bushy Cells in the
 * Ventral Cochlear Nucleus: Implications of a Computational Model", Journal of
 * Neurophysiology, (70), No. 6, pp 2562-2583.
 * It expects the data structure to be passed to the routine as an argument.
 * The function returns TRUE if it succeeds, otherwise it returns FALSE.
 */
 
void
GenerateHHuxley_IonChanList(IonChannelPtr theIC)
{
	/* static const WChar	*funcName = wxT("GenerateHHuxley_IonChanList"); */
	int		j, k;
	double	mV, mDV, alpha, beta, activation, tauActivation;
	ICHHuxleyParsPtr	p;

	p = &theIC->hHuxley;
	mDV = MILLI(theIC->dV);
	for (j = 0, mV = MILLI(theIC->minVoltage); j < theIC->numTableEntries;
	  j++, mV += mDV)
		for (k = 0; k < ICLIST_NUM_GATES; k++) {
			alpha = HHuxleyAlpha_IonChanList(p->aA.array[k], p->aB.array[k],
			  p->aC.array[k], p->aD.array[k], p->aE.array[k],
			  p->aF.array[k], p->aG.array[k], p->aH.array[k],
			  p->aI.array[k], p->aJ.array[k], p->aK.array[k], mV, mDV,
			  theIC->temperature);
			beta = HHuxleyBeta_IonChanList(p->bA.array[k], p->bB.array[k],
			  p->bC.array[k], p->bD.array[k], p->bE.array[k],
			  p->bF.array[k], p->bG.array[k], p->bH.array[k],
			  p->bI.array[k], p->bJ.array[k], p->bK.array[k], mV, mDV,
			  theIC->temperature);
			activation = alpha / (alpha + beta);
			tauActivation = 0.001 / (alpha + beta);
			switch (k) {
			case 0:
				theIC->table[j].yY = activation;
		  		theIC->table[j].ty = tauActivation;
		  		break;
		  	case 1:
				theIC->table[j].zZ = activation;
		  		theIC->table[j].tz = tauActivation;
		  		break;
		  	} /* switch */
		}
	theIC->updateFlag = FALSE;

}

/****************************** GetParsBoltzmann ******************************/

/*
 * This function reads the parameters for the Boltzmann ion channels.
 * It expects the data structure to be passed to the routine as an argument.
 * This ion-channel mode does not use the baseMaxConductance field for the
 * ion-channel structure.
 * The function returns TRUE if it succeeds, otherwise it returns FALSE.
 */
 
BOOLN
GetParsBoltzmann_IonChanList(IonChannelPtr theIC, FILE *fp)
{
	static const WChar	*funcName = wxT("GetParsBoltzmann_IonChanList");
	BOOLN	ok = TRUE;
	int		j;
	ICBoltzmannParsPtr	p;

	p = &theIC->boltzmann;
	if (!GetPars_ParFile(fp, wxT("%lf"), &theIC->conductanceQ10))
		ok = FALSE;
	for (j = 0; j < ICLIST_NUM_GATES; j++)
		if (!GetPars_ParFile(fp, wxT("%lf %lf %lf"),
		  &p->halfMaxV.array[j], &p->zZ.array[j], &p->tau.array[j]))
			ok = FALSE;
	if (!ok) {
		NotifyError(wxT("%s: Could not read parameters for channel '%s'."),
		  funcName, theIC->description);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** GenerateBoltzmann *****************************/

/*
 * This function generates a list of ion channels in an ICList data structure,
 * using the Manis & Marx equations (Manis P. B. and Marx S. O. (1991) "Outward
 * currents in isolated ventral cochlear
 * nucleus neurons", Journal of Neuroscience, (11), pp 2865-2880.
 * It expects the data structure to be passed to the routine as an argument.
 * This ion-channel mode does not use the baseMaxConductance field for the
 * ion-channel structure, but it does read it in, then sets the maxConductance
 * field from it.  This is so that there can be more uniformity in the universal
 * parameters implementation. 
 * The function returns TRUE if it succeeds, otherwise it returns FALSE.
 */
 
void
GenerateBoltzmann_IonChanList(IonChannelPtr theIC)
{
	/* static const WChar	*funcName = wxT("GenerateBoltzmann_IonChanList"); */
	int		j, k;
	double	v, activation, tauActivation, kelvinTemp;
	ICBoltzmannParsPtr	p;

	kelvinTemp = theIC->temperature - ABSOLUTE_ZERO_DEGREES;
	p = &theIC->boltzmann;
	for (j = 0, v = theIC->minVoltage; j < theIC->numTableEntries;
	  j++, v += theIC->dV)
		for (k = 0; k < ICLIST_NUM_GATES; k++) {
			if (p->zZ.array[k] != 0.0) {
				activation = 1.0 / (1.0 + exp(-(v - p->halfMaxV.array[k]) *
				  p->zZ.array[k] * E_OVER_R / kelvinTemp));
				tauActivation = p->tau.array[k] / HHUXLEY_TF(
				  theIC->conductanceQ10, theIC->temperature);
			} else {
				activation = 1.0;
				tauActivation = 1.0;
			}
			switch (k) {
			case 0:
				theIC->table[j].yY = activation;
		  		theIC->table[j].ty = tauActivation;
		  		break;
		  	case 1:
				theIC->table[j].zZ = activation;
		  		theIC->table[j].tz = tauActivation;
		  		break;
		  	} /* switch */
		}
	theIC->updateFlag = FALSE;

}

/****************************** ReadGeneralPars *******************************/

/*
 * This routine reads the general ion channel list parameters from a file
 * stream.
 * It returns with the address of the IonChanList, or NULL if it fails.
 * The base leakage conductance and the leakage conductance are always
 * initialised to the same value.
 */

BOOLN
ReadGeneralPars_IonChanList(FILE *fp, IonChanListPtr theICs)
{
	static const WChar	*funcName = wxT("ReadGeneralPars_IonChanList");
	BOOLN	ok = TRUE;
	WChar	printTablesModeName[SMALL_STRING];
	int		numChannels;
	double	baseLeakageCond, leakagePot, temperature, leakageCondQ10;
	double	minV, maxV, dV;

	if (!GetPars_ParFile(fp, wxT("%s"), printTablesModeName))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &baseLeakageCond))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &leakagePot))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%d"), &numChannels))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &temperature))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &leakageCondQ10))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &minV))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &maxV))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &dV))
		ok = FALSE;
	if (!ok) {
		NotifyError(wxT("%s: Could not read general list parameters."),
		  funcName);
		return(FALSE);
	}
	if (!SetNumChannels_IonChanList(theICs, numChannels))
		ok = FALSE;
	if (!SetPrintTablesMode_IonChanList(theICs, printTablesModeName))
		ok = FALSE;
	if (!SetBaseLeakageCond_IonChanList(theICs, baseLeakageCond))
		ok = FALSE;
	if (!SetLeakagePot_IonChanList(theICs, leakagePot))
		ok = FALSE;
	if (!SetTemperature_IonChanList(theICs, temperature))
		ok = FALSE;
	if (!SetLeakageCondQ10_IonChanList(theICs, leakageCondQ10))
		ok = FALSE;
	if (!SetMinVoltage_IonChanList(theICs, minV))
		ok = FALSE;
	if (!SetMaxVoltage_IonChanList(theICs, maxV))
		ok = FALSE;
	if (!SetVoltageStep_IonChanList(theICs, dV))
		ok = FALSE;
	if (!ok) {
		NotifyError(wxT("%s: Failed to set general parameters."), funcName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** ReadICGeneralPars *****************************/

/*
 * This routine reads the general parameters for an IC structure.
 * It assumes that the IonChannel structure and the IonChanList structure
 * have been correctly initialised.
 * This routine was created so that it could also be used by the
 * 'ResetIonChannel_' routine.
 * If '*fp' is NULL then the filename will not be from the file stream.
 */

BOOLN
ReadICGeneralPars_IonChanList(FILE **fp, ICModeSpecifier mode, WChar *fileName,
  WChar *description, WChar *enabled, double *equilibriumPot,
  double *baseMaxConductance, double *activationExponent)
{
	static const WChar	*funcName = wxT("ReadICGeneralPars_IonChanList");
	BOOLN	ok = TRUE;
	WChar	*filePath;

	if (mode == ICLIST_FILE_MODE) {
		if (*fp && !GetPars_ParFile(*fp, wxT("%s"), fileName)) {
			NotifyError(wxT("%s: Could not read ion channel file '%s'."),
			  funcName, fileName);
			return(FALSE);
		}
		filePath = GetParsFileFPath_Common(fileName);
		if ((*fp = DSAM_fopen(filePath, "r")) == NULL) {
			NotifyError(wxT("%s: Could not open ion channel file '%s'."),
			  funcName, filePath);
			return(FALSE);
		}
	}
	if (!GetPars_ParFile(*fp, wxT("%s"), description))
		ok = FALSE;
	if (!GetPars_ParFile(*fp, wxT("%s"), enabled))
		ok = FALSE;
	if (!GetPars_ParFile(*fp, wxT("%lf"), equilibriumPot))
		ok = FALSE;
	if (!GetPars_ParFile(*fp, wxT("%lf"), baseMaxConductance))
		ok = FALSE;
	if (!GetPars_ParFile(*fp, wxT("%lf"), activationExponent))
		ok = FALSE;
	if (!ok)
		NotifyError(wxT("%s: Could not read general parameters."), funcName);
	return(ok);

}

/****************************** SetICGeneralParsFromICList ********************/

/*
 * This routine sets the general parameters for an IC structure which are
 * inherited from the IC list structure.
 * It assumes that the IonChannel structure and the IonChanList structure
 * have been correctly initialised.
 */

void
SetICGeneralParsFromICList_IonChanList(IonChannelPtr theIC,
  IonChanListPtr theICs)
{
	theIC->temperature = theICs->temperature;
	theIC->minVoltage = theICs->minVoltage;
	theIC->maxVoltage = theICs->maxVoltage;
	theIC->dV = theICs->dV;
	theIC->numTableEntries = theICs->numTableEntries;

}

/****************************** SetICGeneralPars ******************************/

/*
 * This routine sets the general parameters for an IC structure.
 * It assumes that the IonChannel structure and the IonChanList structure
 * have been correctly initialised.
 */

BOOLN
SetICGeneralPars_IonChanList(IonChannelPtr theIC, ICModeSpecifier mode,
  WChar *description, WChar *enabled, double equilibriumPot,
  double baseMaxConductance, double activationExponent)
{
	static const WChar	*funcName = wxT("SetICGeneralPars_IonChanList");
	BOOLN	ok = TRUE;

	if (!SetICDescription_IonChanList(theIC, description))
		ok = FALSE;
	if (!SetICEnabled_IonChanList(theIC, enabled))
		ok = FALSE;
	if (!SetICEquilibriumPot_IonChanList(theIC, equilibriumPot))
		ok = FALSE;
	if (!SetICBaseMaxConductance_IonChanList(theIC, baseMaxConductance))
		ok = FALSE;	
	if (!SetICActivationExponent_IonChanList(theIC, activationExponent))
		ok = FALSE;	
	if (!ok) {
		NotifyError(wxT("%s: Could not set general parameters."), funcName);
		return(FALSE);
	}
	theIC->mode = mode;
	return(TRUE);

}

/****************************** ReadICPars ************************************/

/*
 * This routine reads the ion channel parameters and returns a pointer to an IC
 * structure.
 */

BOOLN
ReadICPars_IonChanList(IonChanListPtr theICs, IonChannelPtr theIC, FILE *fp)
{
	static const WChar	*funcName = wxT("ReadICPars_IonChanList");
	BOOLN	ok = TRUE;
	WChar	fileName[MAX_FILE_PATH], modeName[SMALL_STRING];
	WChar	enabled[SMALL_STRING], description[MAXLINE];
	double	equilibriumPot, baseMaxConductance, activationExponent;
	ICModeSpecifier mode;

	if (!GetPars_ParFile(fp, wxT("%s"), modeName)) {
		NotifyError(wxT("%s: Could not read filename."), funcName);
		return(FALSE);
	}
	if ((mode = (ICModeSpecifier) Identify_NameSpecifier(modeName,
	  iCListModeList)) == ICLIST_NULL) {
		NotifyError(wxT("%s: Unknown ion channel mode (%s)."), funcName,
		  modeName);
		return(FALSE);
	}
	if (!ReadICGeneralPars_IonChanList(&fp, mode, fileName, description,
	  enabled, &equilibriumPot, &baseMaxConductance, &activationExponent)) {
		NotifyError(wxT("%s: Could not read general parameters."), funcName);
		return(FALSE);
	}
	if (!SetICGeneralPars_IonChanList(theIC, mode, description, enabled,
	  equilibriumPot, baseMaxConductance, activationExponent)) {
		NotifyError(wxT("%s: Could not set general parameters."), funcName);
		return(FALSE);
	}
	SetICGeneralParsFromICList_IonChanList(theIC, theICs);
	switch (mode) {
	case ICLIST_BOLTZMANN_MODE:
		if (!GetParsBoltzmann_IonChanList(theIC, fp))
			ok = FALSE;
		else
			GenerateBoltzmann_IonChanList(theIC);
		break;
	case ICLIST_HHUXLEY_MODE:
		if (!GetParsHHuxley_IonChanList(theIC, fp))
			ok = FALSE;
		else
			GenerateHHuxley_IonChanList(theIC);
		break;
	case ICLIST_FILE_MODE:
		DSAM_strncpy(theIC->fileName, fileName, MAX_FILE_PATH);
		if (!ReadVoltageTable_IonChanList(theIC, fp)) {
			NotifyError(wxT("%s: Failed to read ion channel from file '%s'."),
			  funcName, fileName);
			ok = FALSE;
		}
		break;
	default	:
		NotifyError(wxT("%s: Unknown ion channel mode."), funcName);
		ok = FALSE;
		break;
	} /* Switch */
	if (ok && !SetIonChannelUniParListMode_IonChanList(theIC)) { 
		NotifyError(wxT("%s: Could not set parameter list mode."), funcName);
		ok = FALSE;
	}
	SetICPowFunc_IonChanList(theIC);
	return(ok);

}

/****************************** SetGeneratedPars ******************************/

/*
 * This routine sets the generated/calculated parameters for the ion channel
 * list.
 */

BOOLN
SetGeneratedPars_IonChanList(IonChanListPtr theICs)
{
	static const WChar	*funcName = wxT("SetGeneratedPars_IonChanList");

	if (theICs->dV <= DBL_EPSILON) {
		NotifyError(wxT("%s: the voltage step must be greater than zero."),
		  funcName);
		return(FALSE);
	}
	if ((theICs->numTableEntries = (int) ceil((theICs->maxVoltage -
	  theICs->minVoltage) / theICs->dV) + 1) <= 0) {
		NotifyError(wxT("%s: Cannot define table from voltage range\n%g -> %g ")
		  wxT("mV and step size %g mV.\n"), funcName, theICs->minVoltage,
		  theICs->maxVoltage, theICs->dV);
		return(FALSE);
	}
	if (theICs->useTemperatureCalcFlag)
		theICs->leakageCond = theICs->baseLeakageCond *
		  HHUXLEY_TF(theICs->leakageCondQ10, theICs->temperature);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This routine reads the ion channel parameters from a file stream.
 * It returns with the address of the IonChanList, or NULL if it fails.
 * The base leakage conductance and the leakage conductance are always
 * initialised to the same value.
 */

IonChanListPtr
ReadPars_IonChanList(FILE *fp)
{
	static const WChar	*funcName = wxT("ReadPars_IonChanList");
	IonChanListPtr	theICs = NULL;
	IonChannelPtr	theIC;
	DynaListPtr		node;

	if ((theICs = Init_IonChanList((WChar *) funcName)) == NULL) {
		NotifyError(wxT("%s: Out of memory for ion channel list structure."),
		  funcName);
		return(NULL);
	}
	if (!ReadGeneralPars_IonChanList(fp, theICs)) {
		NotifyError(wxT("%s: Failed to read general parameters."), funcName);
		Free_IonChanList(&theICs);
		return(NULL);
	}
	if (!SetGeneratedPars_IonChanList(theICs)) {
		NotifyError(wxT("%s: Failed to set the generated parameters."),
		  funcName);
		Free_IonChanList(&theICs);
		return(NULL);
	}
	if (!PrepareIonChannels_IonChanList(theICs)) {
		NotifyError(wxT("%s: Failed to prepare the ion channels."), funcName);
		Free_IonChanList(&theICs);
		return(NULL);
	}
	for (node = theICs->ionChannels; node; node = node->next) {
		theIC = (IonChannelPtr) node->data;
		if (!ReadICPars_IonChanList(theICs, theIC, fp)) {
			NotifyError(wxT("%s: Could not read ion channel '%s'."), funcName,
			  theIC->description);
			Free_IonChanList(&theICs);
			return(NULL);
		}
	}
	if (!SetGeneralUniParList_IonChanList(theICs)) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_IonChanList(&theICs);
		return(NULL);
	}
	return(theICs);

}

/****************************** GenerateDefault *******************************/

/*
 * This function returns a default ICList structure.
 * It is used to create a structure whose parameters can be set later.
 */

IonChanListPtr
GenerateDefault_IonChanList(void)
{
	static const WChar	*funcName = wxT("GenerateDefault_IonChanList");
	int		i, j;
	IonChanListPtr	theICs = NULL;
	IonChannelPtr	theIC;
	DynaListPtr		node;
	ICBoltzmannParsPtr	p;
	struct iC {
		WChar *	desc;
		double	equilPot;
		double	baseMax;
		double	actExp;
		double	condQ10;
		double	currPars[2][3];
	} iCs[] = {
		{ wxT("Na_Conner"), 0.055, 300e-9, 1.0, 2.5, {{-0.0191, 3.7, 0.10e-3},
		  {-0.053, -3.6, 0.50e-3}}},
		{ wxT("K+_FastHiT"), -0.073, 54e-9, 1.0, 2.5, {{-0.015, 4, 1.61e-3},
		  {0.0, 0.0, 0.0}}},
		{ wxT("K+_SlowLoT"), -0.073, 18e-9, 1.0, 2.5, {{-0.028, 4.6, 10.0e-3},
		  {0.0, 0.0, 0.0}}},
		{ wxT("K+_BoltzmannTr"), -0.073, 24e-9, 1.0, 2.5, {{-0.0302, 0.6,
		  1.0e-3}, {-0.0612, -4.9, 3.0e-3}}}
	};

	if ((theICs = Init_IonChanList((WChar *) funcName)) == NULL) {
		NotifyError(wxT("%s: Out of memory for ion channel list structure."),
		  funcName);
		return(NULL);
	}
	SetNumChannels_IonChanList(theICs, 4);
	SetPrintTablesMode_IonChanList(theICs, wxT("off"));
	SetBaseLeakageCond_IonChanList(theICs, 0.150e-9);
	SetLeakagePot_IonChanList(theICs, -0.01);
	SetTemperature_IonChanList(theICs, 22.0);
	SetLeakageCondQ10_IonChanList(theICs, 2.0);
	SetMinVoltage_IonChanList(theICs, -0.120);
	SetMaxVoltage_IonChanList(theICs, 0.055);
	SetVoltageStep_IonChanList(theICs, 0.001);

	SetGeneratedPars_IonChanList(theICs);
	if (!PrepareIonChannels_IonChanList(theICs)) {
		NotifyError(wxT("%s: Failed to prepare the ion channels."), funcName);
		Free_IonChanList(&theICs);
		return(NULL);
	}
	for (node = theICs->ionChannels, i = 0; node; node = node->next, i++) {
		theIC = (IonChannelPtr) node->data;
		if (!SetICGeneralPars_IonChanList(theIC, ICLIST_BOLTZMANN_MODE,
		  iCs[i].desc, wxT("on"), iCs[i].equilPot, iCs[i].baseMax, iCs[i].
		  actExp)) {
			NotifyError(wxT("%s: Could not set general parameters."), funcName);
			Free_IonChanList(&theICs);
			return(NULL);
		}
		theIC->conductanceQ10 = iCs[i].condQ10;
		p = &theIC->boltzmann;
		for (j = 0; j < ICLIST_NUM_GATES; j++) {
			p->halfMaxV.array[j] = iCs[i].currPars[j][0];
			p->zZ.array[j] = iCs[i].currPars[j][1];
			p->tau.array[j] = iCs[i].currPars[j][2];
		}
		SetICPowFunc_IonChanList(theIC);
		SetIonChannelUniParListMode_IonChanList(theIC);
		GenerateBoltzmann_IonChanList(theIC);
	}
	if (!SetGeneralUniParList_IonChanList(theICs)) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_IonChanList(&theICs);
		return(NULL);
	}
	return(theICs);

}

/****************************** PrepareIonChannels ****************************/

/*
 * This adds a default ion channel to an IC List structure.
 * It is used to create an ion channel structure whose parameters can be set
 * later.
 * If the number of channels is reduced, then the channel at the to of the
 * list is removed.
 */

BOOLN
PrepareIonChannels_IonChanList(IonChanListPtr theICs)
{
	static const WChar	*funcName = wxT("PrepareIonChannels_IonChanList");
	int		i;
	IonChannelPtr	theIC;
	DynaListPtr		node, previous;

	if (!CheckInit_IonChanList(theICs, funcName))
		return(FALSE);

	if (!SetGeneratedPars_IonChanList(theICs)) {
		NotifyError(wxT("%s: Ion channel parameters must be set first."),
		  funcName);
		return(FALSE);
	}
	if (!theICs->ionChannels)
		theICs->oldNumTableEntries = theICs->numTableEntries;
	if (theICs->numChannels != theICs->oldNumChannels) {
		if (theICs->numChannels < theICs->oldNumChannels) {
			node = GetLastInst_Utility_DynaList(theICs->ionChannels);
			for (i = theICs->numChannels; i < theICs->oldNumChannels; i++) {
				theIC = node->data;
				previous = node->previous;
				Remove_Utility_DynaList(&theICs->ionChannels, node);
				FreeIonChannel_IonChanList(&theIC);
				node = previous;
			}
		} else {
			for (i = theICs->oldNumChannels; i < theICs->numChannels; i++) {
				if ((theIC = InitIonChannel_IonChanList(funcName, theICs->
				  numTableEntries)) == NULL)
					return(FALSE);
				SetICGeneralParsFromICList_IonChanList(theIC, theICs);
				if (!Append_Utility_DynaList(&theICs->ionChannels, theIC)) {
					NotifyError(wxT("%s: Could not add ion channel [%d] to ")
					  wxT("list."), funcName, i);
					FreeIonChannel_IonChanList(&theIC);
					return(FALSE);
				}
				if (!SetIonChannelUniParList_IonChanList(theICs, theIC)) { 
					NotifyError(wxT("%s: Could not initialise ion channel ")
					  wxT("'%s' parameter list."), funcName, theIC->description);
					return(FALSE);
				}
			}
		}
		theICs->oldNumChannels = theICs->numChannels;
	}
	if (theICs->numTableEntries != theICs->oldNumTableEntries) {
		for (node = theICs->ionChannels; node; node = node->next) {
			theIC = (IonChannelPtr) node->data;
			theIC->table = (ICTableEntry *) realloc(theIC->table, theICs->
			  numTableEntries * sizeof(ICTableEntry));
		}
		theICs->oldNumTableEntries = theICs->numTableEntries;
	}
	return(TRUE);

}

/****************************** Pow1Func **************************************/

/*
 * This function is a fast implementation of x^1.
 * The second argument is there for compatibility with the standard 'pow'
 * function, but it is not used.
 */

double
Pow1Func_IonChanList(double x, double dummy)
{
	return(x);

}

/****************************** Pow2Func **************************************/

/*
 * This function is a fast implementation of x^2.
 * The second argument is there for compatibility with the standard 'pow'
 * function, but it is not used.
 */

double
Pow2Func_IonChanList(double x, double dummy)
{
	return(x * x);

}

/****************************** Pow3Func **************************************/

/*
 * This function is a fast implementation of x^3.
 * The second argument is there for compatibility with the standard 'pow'
 * function, but it is not used.
 */

double
Pow3Func_IonChanList(double x, double dummy)
{
	return(x * x * x);

}

/****************************** SetICPowFunc **********************************/

/*
 * This routine sets the ion channels Pow function.
 * If the activation is an integer, then the more efficient X * x ,etc. formas
 * are used.
 */

void
SetICPowFunc_IonChanList(IonChannelPtr theIC)
{
	int		intActivation;

	if (fabs(theIC->activationExponent) != (intActivation = (int) floor(fabs(
	  theIC->activationExponent) + 0.5))) {
		theIC->PowFunc = pow;
		return;
	}
	switch (intActivation) {
	case 1:
		theIC->PowFunc = Pow1Func_IonChanList;
		break;
	case 2:
		theIC->PowFunc = Pow2Func_IonChanList;
		break;
	case 3:
		theIC->PowFunc = Pow3Func_IonChanList;
		break;
	default:
		theIC->PowFunc = pow;
	}

}

/****************************** ResetIonChannel *******************************/

/*
 * This routine resets an ion channel for an ion channel list.
 * It returns false if it fails in any way.
 */

BOOLN
ResetIonChannel_IonChanList(IonChanListPtr theICs, IonChannelPtr theIC)
{
	static const WChar	*funcName = wxT("ResetIonChannel_IonChanList");
	BOOLN	ok = TRUE;
	WChar	enabled[SMALL_STRING], description[MAXLINE];
	double	equilibriumPot, baseMaxConductance, activationExponent;
	FILE	*fp = NULL;

	if (!CheckInitIC_IonChanList(theIC, funcName))
		return(FALSE);
	switch (theIC->mode) {
	case ICLIST_FILE_MODE:
		Init_ParFile();
		if (!ReadICGeneralPars_IonChanList(&fp, theIC->mode,
		  theIC->fileName, description, enabled, &equilibriumPot,
		  &baseMaxConductance, &activationExponent)) {
			NotifyError(wxT("%s: Could not read general parameters."),
			  funcName);
			ok = FALSE;
		}
		if (ok && !SetICGeneralPars_IonChanList(theIC, theIC->mode, description,
		  enabled, equilibriumPot, baseMaxConductance, activationExponent)) {
			NotifyError(wxT("%s: Could not set general parameters."), funcName);
			FreeIonChannel_IonChanList(&theIC);
			ok = FALSE;
		}
		SetICGeneralParsFromICList_IonChanList(theIC, theICs);
		if (ok)
			ok = ReadVoltageTable_IonChanList(theIC, fp);
		Free_ParFile();
		break;
	case ICLIST_BOLTZMANN_MODE:
		GenerateBoltzmann_IonChanList(theIC);
		break;
	case ICLIST_HHUXLEY_MODE:
		GenerateHHuxley_IonChanList(theIC);
		break;
	default	:
		NotifyError(wxT("%s: Ion channel mode not implemented (%d)."), funcName,
		  theIC->mode);
		ok = FALSE;
	} /* Switch */
	SetICPowFunc_IonChanList(theIC);
	return(ok);

}

/****************************** CheckInit *************************************/

/*
 * This routine checks whether or not an ion channel list.has been initialised.
 */

BOOLN
CheckInit_IonChanList(IonChanListPtr theICList, const WChar *callingFunction)
{
	static const WChar	*funcName = wxT("CheckInit_IonChanList");

	if (theICList == NULL) {
		NotifyError(wxT("%s: ICList not set in %s."), funcName,
		  callingFunction);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** CheckInitIC ***********************************/

/*
 * This routine checks whether or not an ion channel has been initialised.
 */

BOOLN
CheckInitIC_IonChanList(IonChannelPtr theIC, const WChar *callingFunction)
{
	static const WChar	*funcName = wxT("CheckInitIC_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised in %s."), funcName,
		  callingFunction);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** FindVoltageIndex ******************************/

/*
 * This function returns a pointer to an entry in a table, corresponding to a 
 * specified voltage.
 * There will be a fatal error if the voltage is outside the bounds of the
 * table.
 * The "floor(..)" function is used for the rounding, because in biological
 * systems sub-threshold quantities do not fire, and leak away.
 */

ICTableEntryPtr
GetTableEntry_IonChanList(IonChannelPtr theIC, double voltage)
{
	static const WChar	*funcName = wxT("GetTableEntry_IonChanList");
	int		index;
	
	index = (int) floor((voltage - theIC->minVoltage) / theIC->dV + 0.5);
	if ((index < 0) || (index >= theIC->numTableEntries)) {
		NotifyError(wxT("%s: Voltage is outside the table bounds for\nchannel ")
		  wxT("'%s' (%g mV)."), funcName, theIC->description, MILLI(voltage));
		return(NULL);
	}
	return(&theIC->table[index]);
	
}

/********************************* SetNumChannels *****************************/

/*
 * This routine sets the numChannels field of an IonChanList.
 */

BOOLN
SetNumChannels_IonChanList(IonChanListPtr theICs, int numChannels)
{
	static const WChar *funcName = wxT("SetNumChannels_IonChanList");
	DynaListPtr	node;

	if (!CheckInit_IonChanList(theICs, funcName))
		return(FALSE);
	if (numChannels < 1) {
		NotifyError(wxT("%s: Insufficient channels (%d)."), funcName,
		  numChannels);
		return(FALSE);
	}
	theICs->numChannels = numChannels;
	theICs->updateFlag = TRUE;
	if (theICs->ionChannels)
		for (node = theICs->ionChannels; node; node = node->next)
			((IonChannelPtr) node->data)->description[0] = '\0';
	return(TRUE);

}

/********************************* SetTemperature *****************************/

/*
 * This routine sets the temperature field of an IonChanList.
 */

BOOLN
SetTemperature_IonChanList(IonChanListPtr theICs, double theTemperature)
{
	static const WChar *funcName = wxT("SetTemperature_IonChanList");

	if (!CheckInit_IonChanList(theICs, funcName))
		return(FALSE);
	theICs->temperature = theTemperature;
	theICs->updateFlag = TRUE;
	return(TRUE);

}

/********************************* SetLeakageCondQ10 **************************/

/*
 * This routine sets the leakageCondQ10 field of an IonChanList.
 */

BOOLN
SetLeakageCondQ10_IonChanList(IonChanListPtr theICs, double theLeakageCondQ10)
{
	static const WChar *funcName = wxT("SetLeakageCondQ10_IonChanList");

	if (!CheckInit_IonChanList(theICs, funcName))
		return(FALSE);
	theICs->leakageCondQ10 = theLeakageCondQ10;
	return(TRUE);

}

/********************************* SetPrintTablesMode *************************/

/*
 * This routine sets the Print Tables Mode of an IonChanList.
 */

BOOLN
SetPrintTablesMode_IonChanList(IonChanListPtr theICs, WChar *modeName)
{
	static const WChar *funcName = wxT("SetPrintTablesMode_IonChanList");

	if (!CheckInit_IonChanList(theICs, funcName))
		return(FALSE);
	theICs->printTablesMode = IdentifyDiag_NSpecLists(modeName,
	  theICs->printTablesModeList);
	return(TRUE);

}

/********************************* SetBaseLeakageCond *************************/

/*
 * This routine sets the baseLeakageCond field of an IonChanList.
 */

BOOLN
SetBaseLeakageCond_IonChanList(IonChanListPtr theICs, double baseLeakageCond)
{
	static const WChar *funcName = wxT("SetBaseLeakageCond_IonChanList");

	if (!CheckInit_IonChanList(theICs, funcName))
		return(FALSE);
	theICs->leakageCond = theICs->baseLeakageCond = baseLeakageCond;
	return(TRUE);

}

/********************************* SetLeakagePot ******************************/

/*
 * This routine sets the leakagePot field of an IonChanList.
 */

BOOLN
SetLeakagePot_IonChanList(IonChanListPtr theICs, double leakagePot)
{
	static const WChar *funcName = wxT("SetBaseLeakageCond_IonChanList");

	if (!CheckInit_IonChanList(theICs, funcName))
		return(FALSE);
	theICs->leakagePot = leakagePot;
	return(TRUE);

}

/********************************* SetMinVoltage ******************************/

/*
 * This routine sets the minimum voltage parameter for the ion channel list.
 */
 
BOOLN
SetMinVoltage_IonChanList(IonChanListPtr theICs, double theMinVoltage)
{
	static const WChar *funcName = wxT("SetMinVoltage_IonChanList");

	if (!CheckInit_IonChanList(theICs, funcName))
		return(FALSE);
	theICs->minVoltage = theMinVoltage;
	theICs->updateFlag = TRUE;
	return(TRUE);

}

/********************************* SetMaxVoltage ******************************/

/*
 * This routine sets the maximum voltage parameter for the ion channel list.
 */
 
BOOLN
SetMaxVoltage_IonChanList(IonChanListPtr theICs, double theMaxVoltage)
{
	static const WChar *funcName = wxT("SetMaxVoltage_IonChanList");

	if (!CheckInit_IonChanList(theICs, funcName))
		return(FALSE);
	theICs->maxVoltage = theMaxVoltage;
	theICs->updateFlag = TRUE;
	return(TRUE);

}

/********************************* SetVoltageStep *****************************/

/*
 * This routine sets the voltage step parameter for the ion channel list.
 */
 
BOOLN
SetVoltageStep_IonChanList(IonChanListPtr theICs, double theVoltageStep)
{
	static const WChar *funcName = wxT("SetVoltageStep_IonChanList");

	if (!CheckInit_IonChanList(theICs, funcName))
		return(FALSE);
	theICs->dV = theVoltageStep;
	theICs->updateFlag = TRUE;
	return(TRUE);

}

/****************************** SetICEnabled **********************************/

/*
 * This function sets the module's operationMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetICEnabled_IonChanList(IonChannelPtr theIC, WChar *theICEnabled)
{
	static const WChar	*funcName = wxT("SetICEnabled_IonChanList(");
	int		specifier;

	if (!CheckInitIC_IonChanList(theIC, funcName))
		return(FALSE);
	if ((specifier = Identify_NameSpecifier(theICEnabled,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal status name (%s)."), funcName,
		  theICEnabled);
		return(FALSE);
	}
	theIC->enabled = specifier;
	return(TRUE);

}

/********************************* SetICMode **********************************/

/*
 * This routine sets the diagnostic mode of an IonChanList.
 */

BOOLN
SetICMode_IonChanList(IonChannelPtr theIC, WChar *modeName)
{
	static const WChar *funcName = wxT("SetICMode_IonChanList");
	int		mode;

	if (!CheckInitIC_IonChanList(theIC, funcName))
		return(FALSE);
	if ((mode = Identify_NameSpecifier(modeName, iCListModeList)) ==
	  ICLIST_NULL) {
		NotifyError(wxT("%s: Unknown ion channel mode (%s)."), funcName,
		  modeName);
		return(FALSE);
	}
	theIC->mode = (ICModeSpecifier) mode;
	if (theIC->parList) {
		SetIonChannelUniParListMode_IonChanList(theIC);
		theIC->parList->updateFlag = TRUE;
	}
	theIC->updateFlag = TRUE;
	return(TRUE);

}

/********************************* SetICDescription ***************************/

/*
 * This routine sets the specifier ion channel's description parameter.
 */
 
BOOLN
SetICDescription_IonChanList(IonChannelPtr theIC, WChar *theDescription)
{
	static const WChar *funcName = wxT("SetICDescription_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	DSAM_strncpy(theIC->description, theDescription, MAXLINE);
	return(TRUE);

}

/********************************* SetICEquilibriumPot ************************/

/*
 * This routine sets the specifier ion channel's EquilibriumPot parameter.
 */
 
BOOLN
SetICEquilibriumPot_IonChanList(IonChannelPtr theIC, double theEquilibriumPot)
{
	static const WChar *funcName = wxT("SetICEquilibriumPot_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->equilibriumPot = theEquilibriumPot;
	return(TRUE);

}

/********************************* SetICBaseMaxConductance ********************/

/*
 * This routine sets the specifier ion channel's baseMaxConductance parameter.
 */
 
BOOLN
SetICBaseMaxConductance_IonChanList(IonChannelPtr theIC,
  double theBaseMaxConductance)
{
	static const WChar *funcName = wxT("SetICBaseMaxConductance_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->baseMaxConductance = theBaseMaxConductance;
	switch (theIC->mode) {
	case ICLIST_BOLTZMANN_MODE:
	case ICLIST_HHUXLEY_MODE:
		theIC->maxConductance = theIC->baseMaxConductance *
		  HHUXLEY_TF(theIC->conductanceQ10, theIC->temperature);
	default:
		theIC->maxConductance = theIC->baseMaxConductance;
	}
	return(TRUE);

}

/********************************* SetICConductanceQ10 ************************/

/*
 * This routine sets the specifier ion channel's conductanceQ10 parameter.
 */
 
BOOLN
SetICConductanceQ10_IonChanList(IonChannelPtr theIC, double theConductanceQ10)
{
	static const WChar *funcName = wxT("SetICConductanceQ10_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->conductanceQ10 = theConductanceQ10;
	return(TRUE);

}

/********************************* SetICActivationExponent ********************/

/*
 * This routine sets the specifier ion channel's activationExponent parameter.
 */
 
BOOLN
SetICActivationExponent_IonChanList(IonChannelPtr theIC,
  double theActivationExponent)
{
	static const WChar *funcName = wxT("SetICActivationExponent_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->activationExponent = theActivationExponent;
	return(TRUE);

}

/********************************* SetICBoltzmannHalfMaxV *********************/

/*
 * This routine sets the specifier ion channel's halfMaxV array element.
 */
 
BOOLN
SetICBoltzmannHalfMaxV_IonChanList(IonChannelPtr theIC, int index,
  double theHalfMaxV)
{
	static const WChar *funcName = wxT("SetICBoltzmannHalfMaxV_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->boltzmann.halfMaxV.array[index] = theHalfMaxV;
	return(TRUE);

}

/********************************* SetICBoltzmannZ ****************************/

/*
 * This routine sets the specifier ion channel's zZ array element.
 */
 
BOOLN
SetICBoltzmannZ_IonChanList(IonChannelPtr theIC, int index, double theZ)
{
	static const WChar *funcName = wxT("SetICBoltzmannZ_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->boltzmann.zZ.array[index] = theZ;
	return(TRUE);

}

/********************************* SetICBoltzmannTau **************************/

/*
 * This routine sets the specifier ion channel's tau array element.
 */
 
BOOLN
SetICBoltzmannTau_IonChanList(IonChannelPtr theIC, int index, double theTau)
{
	static const WChar *funcName = wxT("SetICBoltzmannTau_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->boltzmann.tau.array[index] = theTau;
	return(TRUE);

}

/********************************* SetICHHuxleyAlphaA *************************/

/*
 * This routine sets the specifier ion channel's AlphaA array element.
 */
 
BOOLN
SetICHHuxleyAlphaA_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyAlphaA_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.aA.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyAlphaB *************************/

/*
 * This routine sets the specifier ion channel's AlphaB array element.
 */
 
BOOLN
SetICHHuxleyAlphaB_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyAlphaB_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.aB.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyAlphaC *************************/

/*
 * This routine sets the specifier ion channel's AlphaC array element.
 */
 
BOOLN
SetICHHuxleyAlphaC_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyAlphaC_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.aC.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyAlphaD *************************/

/*
 * This routine sets the specifier ion channel's AlphaD array element.
 */
 
BOOLN
SetICHHuxleyAlphaD_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyAlphaD_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.aD.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyAlphaE *************************/

/*
 * This routine sets the specifier ion channel's AlphaE array element.
 */
 
BOOLN
SetICHHuxleyAlphaE_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyAlphaE_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.aE.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyAlphaF *************************/

/*
 * This routine sets the specifier ion channel's AlphaF array element.
 */
 
BOOLN
SetICHHuxleyAlphaF_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyAlphaF_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.aF.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyAlphaG *************************/

/*
 * This routine sets the specifier ion channel's AlphaG array element.
 */
 
BOOLN
SetICHHuxleyAlphaG_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyAlphaG_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.aG.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyAlphaH *************************/

/*
 * This routine sets the specifier ion channel's AlphaH array element.
 */
 
BOOLN
SetICHHuxleyAlphaH_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyAlphaH_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.aH.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyAlphaI *************************/

/*
 * This routine sets the specifier ion channel's AlphaI array element.
 */
 
BOOLN
SetICHHuxleyAlphaI_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyAlphaI_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.aI.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyAlphaJ *************************/

/*
 * This routine sets the specifier ion channel's AlphaJ array element.
 */
 
BOOLN
SetICHHuxleyAlphaJ_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyAlphaJ_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.aJ.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyAlphaK *************************/

/*
 * This routine sets the specifier ion channel's AlphaK array element.
 */
 
BOOLN
SetICHHuxleyAlphaK_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyAlphaK_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.aK.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyBetaA **************************/

/*
 * This routine sets the specifier ion channel's BetaA array element.
 */
 
BOOLN
SetICHHuxleyBetaA_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyBetaA_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.bA.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyBetaB **************************/

/*
 * This routine sets the specifier ion channel's BetaB array element.
 */
 
BOOLN
SetICHHuxleyBetaB_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyBetaB_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.bB.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyBetaC **************************/

/*
 * This routine sets the specifier ion channel's BetaC array element.
 */
 
BOOLN
SetICHHuxleyBetaC_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyBetaC_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.bC.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyBetaD **************************/

/*
 * This routine sets the specifier ion channel's BetaD array element.
 */
 
BOOLN
SetICHHuxleyBetaD_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyBetaD_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.bD.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyBetaE **************************/

/*
 * This routine sets the specifier ion channel's BetaE array element.
 */
 
BOOLN
SetICHHuxleyBetaE_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyBetaE_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.bE.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyBetaF **************************/

/*
 * This routine sets the specifier ion channel's BetaF array element.
 */
 
BOOLN
SetICHHuxleyBetaF_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyBetaF_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.bF.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyBetaG **************************/

/*
 * This routine sets the specifier ion channel's BetaG array element.
 */
 
BOOLN
SetICHHuxleyBetaG_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyBetaG_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.bG.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyBetaH **************************/

/*
 * This routine sets the specifier ion channel's BetaH array element.
 */
 
BOOLN
SetICHHuxleyBetaH_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyBetaH_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.bH.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyBetaI **************************/

/*
 * This routine sets the specifier ion channel's BetaI array element.
 */
 
BOOLN
SetICHHuxleyBetaI_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyBetaI_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.bI.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyBetaJ **************************/

/*
 * This routine sets the specifier ion channel's BetaJ array element.
 */
 
BOOLN
SetICHHuxleyBetaJ_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyBetaJ_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.bJ.array[index] = value;
	return(TRUE);

}

/********************************* SetICHHuxleyBetaK **************************/

/*
 * This routine sets the specifier ion channel's BetaK array element.
 */
 
BOOLN
SetICHHuxleyBetaK_IonChanList(IonChannelPtr theIC, int index, double value)
{
	static const WChar *funcName = wxT("SetICHHuxleyBetaK_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->hHuxley.bK.array[index] = value;
	return(TRUE);

}

/********************************* SetICFileName ******************************/

/*
 * This routine sets the specifier ion channel table file name.
 */
 
BOOLN
SetICFileName_IonChanList(IonChannelPtr theIC, WChar *fileName)
{
	static const WChar *funcName = wxT("SetICFileName_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	DSAM_strncpy(theIC->fileName, fileName, MAX_FILE_PATH);
	theIC->updateFlag = TRUE;
	if (theIC->parList)
		theIC->parList->updateFlag = TRUE;
	return(TRUE);

}


