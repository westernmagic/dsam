/**********************
 *
 * File:		AudPer86.c
 * Purpose:		This is a simple test program, running an auditory periphery
 *				model.  It has been written using hard programming.
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

#define	PARAMETERS_FILE		"AudPer86.par"	/* Name of paramters file.*/
#define	NUM_CHANNELS		1			/* No. of channels for the filter. */
#define CHANNEL				0			/* Channel no. for test. */

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

char	outputFile[MAXLINE], stParFile[MAXLINE], pEParFile[MAXLINE];
char	bMParFile[MAXLINE], hCParFile[MAXLINE];

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
	GetPars_ParFile(fp, "%s", stParFile);
	GetPars_ParFile(fp, "%s", pEParFile);
	GetPars_ParFile(fp, "%s", bMParFile);
	GetPars_ParFile(fp, "%s", hCParFile);
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
	EarObjectPtr	hairCell = NULL, intensity = NULL;
	
	printf("Starting Test Harness...\n");
	
	ReadParsFromFile(PARAMETERS_FILE);
	printf("\nIn this test a pure tone stimulus is presented to an\n");
	printf("auditory periphery model.\n");
	printf("A rise time of %g ms is applied to the stimulus.\n",
	  MSEC(rampInterval));
	printf("\n");
	
	/* Initialise EarObjects. */
	
	stimulus = Init_EarObject( "null" );
	pEFilter = Init_EarObject( "null" );
	bMFilter = Init_EarObject( "null" );
	hairCell = Init_EarObject( "null" );
	intensity = Init_EarObject( "null" );

	/* Set up EarObject connections. */
	
	ConnectOutSignalToIn_EarObject( stimulus, intensity );

	ConnectOutSignalToIn_EarObject( stimulus, pEFilter );
	ConnectOutSignalToIn_EarObject( pEFilter, bMFilter );
	ConnectOutSignalToIn_EarObject( bMFilter, hairCell );

	/* Initialise modules. */

	printf("Module parameters...\n\n" );

	Init_PureTone( GLOBAL );
	ReadPars_PureTone( stParFile );
	PrintPars_PureTone();

	Init_PreEmphasis_BandPass( GLOBAL );
	ReadPars_PreEmphasis_BandPass( pEParFile );
	PrintPars_PreEmphasis_BandPass();

	Init_BasilarM_GammaT( GLOBAL );
	ReadPars_BasilarM_GammaT( bMParFile );
	PrintPars_BasilarM_GammaT();

	Init_IHC_Meddis86( GLOBAL );
	ReadPars_IHC_Meddis86( hCParFile );
	PrintPars_IHC_Meddis86();
	
	Init_Analysis_Intensity( GLOBAL );
	SetTimeOffset_Analysis_Intensity( rampInterval );
	PrintPars_Analysis_Intensity();

	/* Start main process and print diagonstics. */
	
	printf("\nStarting main process...\n\n" );
	GenerateSignal_PureTone( stimulus );
	PrintProcessName_EarObject( "1-stimulus: '%s'.\n", stimulus );
	RampUpOutSignal_Ramp( stimulus, Sine_Ramp, rampInterval );
	Calc_Analysis_Intensity( intensity );
	printf("\tStimulus intensity = %g dB SPL.\n",
	  GetResult_EarObject(intensity, CHANNEL ));
	RunModel_PreEmphasis_BandPass( pEFilter );
	PrintProcessName_EarObject("2-Outer-/middle-ear: '%s'.\n", pEFilter );
	RunModel_BasilarM_GammaT( bMFilter );
	PrintProcessName_EarObject("3-Basilar membrane: '%s'.\n", bMFilter );
	RunModel_IHC_Meddis86( hairCell );
	PrintProcessName_EarObject("4-Inner hair cell (IHC): '%s'.\n", hairCell );
	WriteOutSignal_DataFile(outputFile, hairCell );

	FreeAll_EarObject();

	printf("Finished test.\n");
    return(0);
	
}
