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
#define	INTENSITY_MOD_NAME	"Analysis_Intensity"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

char	outputFile[MAXLINE], stParFile[MAXLINE], pEParFile[MAXLINE];
char	bMParFile[MAXLINE], rPParFile[MAXLINE], hCParFile[MAXLINE];

char	stModuleName[MAXLINE], pEModuleName[MAXLINE], bMModuleName[MAXLINE];
char	rPModuleName[MAXLINE], hCModuleName[MAXLINE];

double	rampInterval;

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
	GetPars_ParFile(fp, "%s %s", pEParFile, pEModuleName);
	GetPars_ParFile(fp, "%s %s", bMParFile, bMModuleName);
	GetPars_ParFile(fp, "%s %s", rPParFile, rPModuleName);
	GetPars_ParFile(fp, "%s %s", hCParFile, hCModuleName);
	GetPars_ParFile(fp, "%lf", &rampInterval);
	fclose(fp);
	Free_ParFile();
	
}

/******************************************************************************/
/****************************** Main Body *************************************/
/******************************************************************************/

int main(void)
{
	EarObjectPtr	stimulus = NULL, pEFilter = NULL, bMFilter = NULL;
	EarObjectPtr	recptPotn = NULL, hairCell = NULL, intensity = NULL;
	
	printf("Starting Test Harness...\n");
	
	ReadParsFromFile(PARAMETERS_FILE);
	printf("\nIn this test a '%s' stimulus is presented to an\n", stModuleName);
	printf("auditory periphery model.\n");
	printf("If necessary, a rise time of %g ms is applied to the stimulus.\n",
	  MSEC(rampInterval));
	printf("The model process contains the following modules:\n\n");
	printf("\tStimulus generation:\t%s\n", stModuleName);
	printf("\tOuter-/middle-ear:\t%s\n", pEModuleName);
	printf("\tBasilar membrane:\t%s\n", bMModuleName);
	printf("\tIHC receptor pot.:\t%s\n", rPModuleName);
	printf("\tInner hair cell (IHC):\t%s\n", hCModuleName);
	printf("\n");
	
	/* Initialise EarObjects. */
	
	stimulus = Init_EarObject(stModuleName);
	pEFilter = Init_EarObject(pEModuleName);
	bMFilter = Init_EarObject(bMModuleName);
	recptPotn = Init_EarObject(rPModuleName);
	hairCell = Init_EarObject(hCModuleName);
	intensity = Init_EarObject(INTENSITY_MOD_NAME);

	/* Set up EarObject connections. */
	
	ConnectOutSignalToIn_EarObject( stimulus, intensity );

	ConnectOutSignalToIn_EarObject( stimulus, pEFilter );
	ConnectOutSignalToIn_EarObject( pEFilter, bMFilter );
	ConnectOutSignalToIn_EarObject( bMFilter, recptPotn );
	ConnectOutSignalToIn_EarObject( recptPotn, hairCell );

	/* Initialise modules. */

	printf("Module parameters...\n\n" );

	DoFun1( ReadPars, stimulus, stParFile);
	DoFun( PrintPars, stimulus );

	DoFun1( ReadPars, pEFilter, pEParFile);
	DoFun( PrintPars, pEFilter );

	DoFun1( ReadPars, bMFilter, bMParFile);
	DoFun( PrintPars, bMFilter );

	DoFun1( ReadPars, recptPotn, rPParFile);
	DoFun( PrintPars, recptPotn );
	
	DoFun1( ReadPars, hairCell, hCParFile);
	DoFun( PrintPars, hairCell );
	
	DoFun1(SetTimeOffset, intensity, rampInterval);
	DoFun( PrintPars, intensity );
	
	/* Start main process and print diagonstics. */
	
	printf("\nStarting main process...\n\n" );
	DoProcess( stimulus );
	PrintProcessName_EarObject("1-stimulus: '%s'.\n", stimulus );
	if (!stimulus->outSignal->rampFlag ) {
		RampUpOutSignal_Ramp(stimulus, Sine_Ramp, rampInterval );
		printf("\tAudPerGen: Stimulus has been ramped.\n" );
	}
	DoProcess(intensity);
	printf("\tStimulus intensity = %g dB SPL.\n",
	  GetResult_EarObject( intensity, CHANNEL ) );
	DoProcess( pEFilter );
	PrintProcessName_EarObject("2-Outer-/middle-ear: '%s'.\n", pEFilter );
	DoProcess( bMFilter );
	PrintProcessName_EarObject("3-Basilar membrane: '%s'.\n", bMFilter );
	DoProcess( recptPotn );
	PrintProcessName_EarObject("4-IHC Receptor potential: '%s'.\n", recptPotn );
	DoProcess( hairCell );
	PrintProcessName_EarObject("5-Inner hair cell (IHC): '%s'.\n", hairCell );
	WriteOutSignal_DataFile(outputFile, hairCell );

	FreeAll_EarObject();

	printf("Finished test.\n");
    exit(0);
	
}
