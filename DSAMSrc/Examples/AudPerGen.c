/**********************
 *
 * File:		AudPerGen.c
 * Purpose:		This is a simple test program, running an auditory periphery
 *				model.  It has been written using generic programming.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		27 Jan 1996
 * Copyright:	(c) 1997, University of Essex
 *
 **********************/

#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "DSAM.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	PARAMETERS_FILE		"AudPerGen.par"	/* Name of paramters file.*/
#define	NUM_CHANNELS		1			/* No. of channels for the filter. */
#define CHANNEL				0			/* Channel no. for test. */
#define	INTENSITY_MOD_NAME	"Ana_Intensity"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

char	outputFile[MAXLINE], stParFile[MAXLINE], trParFile[MAXLINE];
char	pEParFile[MAXLINE], bMParFile[MAXLINE], rPParFile[MAXLINE];
char	hCParFile[MAXLINE];

char	stModuleName[MAXLINE], trModuleName[MAXLINE], pEModuleName[MAXLINE];
char	bMModuleName[MAXLINE], rPModuleName[MAXLINE], hCModuleName[MAXLINE];

/******************************************************************************/
/****************************** Functions and subroutines *********************/
/******************************************************************************/

/****************************** ReadParsFromFile ******************************/

/*
 * This program reads a specified number of parameters from a file.
 * It expects there to be one parameter per line.
 */
 
void
ReadParsFromFile(char *fileName)
{
	FILE	*fp;
	
	if ((fp = fopen(fileName, "r")) == NULL) {
		NotifyError("ReadTestPars: Cannot open data file '%s'.\n", fileName);
		exit(1);
	}
	printf("Reading parameters from file: %s\n", fileName);
	Init_ParFile();
	GetPars_ParFile(fp, "%s", outputFile);
	GetPars_ParFile(fp, "%s %s", stParFile, stModuleName);
	GetPars_ParFile(fp, "%s %s", trParFile, trModuleName);
	GetPars_ParFile(fp, "%s %s", pEParFile, pEModuleName);
	GetPars_ParFile(fp, "%s %s", bMParFile, bMModuleName);
	GetPars_ParFile(fp, "%s %s", rPParFile, rPModuleName);
	GetPars_ParFile(fp, "%s %s", hCParFile, hCModuleName);
	fclose(fp);
	Free_ParFile();
	
}

/******************************************************************************/
/****************************** Main Body *************************************/
/******************************************************************************/

int main(void)
{
	EarObjectPtr	stimulus = NULL, gate = NULL, pEFilter = NULL;
	EarObjectPtr	bMFilter = NULL, recptPotn = NULL, hairCell = NULL;
	EarObjectPtr	intensity = NULL;
	UniParPtr		par = NULL;
	
	printf("Starting Test Harness...\n");
	
	ReadParsFromFile(PARAMETERS_FILE);
	printf("\nIn this test a '%s' stimulus is presented to an\n", stModuleName);
	printf("auditory periphery model.\n");
	printf("The model process contains the following modules:\n\n");
	printf("\tStimulus generation:\t%s\n", stModuleName);
	printf("\tGate transformation:\t%s\n", trModuleName);
	printf("\tOuter-/middle-ear:\t%s\n", pEModuleName);
	printf("\tBasilar membrane:\t%s\n", bMModuleName);
	printf("\tIHC receptor pot.:\t%s\n", rPModuleName);
	printf("\tInner hair cell (IHC):\t%s\n", hCModuleName);
	printf("\n");
	
	/* Initialise EarObjects. */
	
	stimulus = Init_EarObject(stModuleName);
	gate = Init_EarObject(trModuleName);
	pEFilter = Init_EarObject(pEModuleName);
	bMFilter = Init_EarObject(bMModuleName);
	recptPotn = Init_EarObject(rPModuleName);
	hairCell = Init_EarObject(hCModuleName);
	intensity = Init_EarObject(INTENSITY_MOD_NAME);

	/* Set up EarObject connections. */
	
	ConnectOutSignalToIn_EarObject( stimulus, intensity );

	ConnectOutSignalToIn_EarObject( stimulus, gate );
	ConnectOutSignalToIn_EarObject( gate, pEFilter );
	ConnectOutSignalToIn_EarObject( pEFilter, bMFilter );
	ConnectOutSignalToIn_EarObject( bMFilter, recptPotn );
	ConnectOutSignalToIn_EarObject( recptPotn, hairCell );

	/* Initialise modules. */

	printf("Module parameters...\n\n" );

	ReadPars_ModuleMgr( stimulus, stParFile);
	PrintPars_ModuleMgr( stimulus );

	ReadPars_ModuleMgr( gate, trParFile);
	PrintPars_ModuleMgr( gate );

	ReadPars_ModuleMgr( pEFilter, pEParFile);
	PrintPars_ModuleMgr( pEFilter );

	ReadPars_ModuleMgr( bMFilter, bMParFile);
	PrintPars_ModuleMgr( bMFilter );

	ReadPars_ModuleMgr( recptPotn, rPParFile);
	PrintPars_ModuleMgr( recptPotn );
	
	ReadPars_ModuleMgr( hairCell, hCParFile);
	PrintPars_ModuleMgr( hairCell );

	par = GetUniParPtr_ModuleMgr(gate, "duration");
	SetRealPar_ModuleMgr(intensity, "offset", *par->valuePtr.r);
	PrintPars_ModuleMgr( intensity );
	
	/* Start main process and print diagonstics. */
	
	printf("\nStarting main process...\n\n" );
	RunProcess_ModuleMgr( stimulus );
	PrintProcessName_EarObject("1-stimulus: '%s'.\n", stimulus );
	RunProcess_ModuleMgr( gate );
	PrintProcessName_EarObject("2-gate: '%s'.\n", gate );
	RunProcess_ModuleMgr( intensity );
	printf("\tStimulus intensity = %g dB SPL.\n",
	  GetResult_EarObject( intensity, CHANNEL ) );
	RunProcess_ModuleMgr( pEFilter );
	PrintProcessName_EarObject("3-Outer-/middle-ear: '%s'.\n", pEFilter );
	RunProcess_ModuleMgr( bMFilter );
	PrintProcessName_EarObject("5-Basilar membrane: '%s'.\n", bMFilter );
	RunProcess_ModuleMgr( recptPotn );
	PrintProcessName_EarObject("4-IHC Receptor potential: '%s'.\n", recptPotn );
	RunProcess_ModuleMgr( hairCell );
	PrintProcessName_EarObject("6-Inner hair cell (IHC): '%s'.\n", hairCell );
	WriteOutSignal_DataFile(outputFile, hairCell );

	FreeAll_EarObject();

	printf("Finished test.\n");
    exit(0);
	
}
