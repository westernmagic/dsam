/**********************
 *
 * File:		AudPerSS.c
 * Purpose:		This is a simple test program, running an auditory periphery
 *				model.  It has been written using simulation specification
 *				programming.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		27 Jan 1996
 * Updated:		
 * Copyright:	(c) 1997, University of Essex
 *
 *********************/

#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "DSAM.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	PARAMETERS_FILE		"AudPerSS.par"	/* Name of paramters file.*/
#define	NUM_CHANNELS		1				/* Number of signal channels. */
#define	CHANNEL				0				/* Channel for testing. */
#define	SIMULATION_SPEC_MOD_NAME	"Util_SimScript"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

char	simScriptFile[MAXLINE];

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
	GetPars_ParFile(fp, "%s", simScriptFile);
	fclose(fp);
	Free_ParFile();
	
}

/******************************************************************************/
/****************************** Main Body *************************************/
/******************************************************************************/

int MainSimulation(void)
{
	clock_t		startTime;
	EarObjectPtr	audModel = NULL;
	
	DPrint("Starting Test Harness...\n");
	
	ReadParsFromFile(PARAMETERS_FILE);
	DPrint("This test routine calculates the response of a model.\n");
	DPrint("This simulation is run from the specification in file '%s'.\n",
	  simScriptFile);
	DPrint("\n");
	
	/* Initialise EarObjects. */
	if (( audModel = Init_EarObject(SIMULATION_SPEC_MOD_NAME )) == NULL)
		exit(1);

	if (!ReadPars_ModuleMgr( audModel, simScriptFile ))
		exit(1);
	PrintPars_ModuleMgr( audModel );
	
	DPrint("Starting test harness...\n");
	startTime = clock();
	RunProcess_ModuleMgr( audModel );
	
	DPrint("The process took %g CPU seconds to run.\n", (double) (clock() -
	  startTime) / CLOCKS_PER_SEC);
	FreeAll_EarObject();
	
	DPrint("Finished test.\n");
	return(0);
	
}
