/*******
 *
 * Simple1.c
 *
 ********/

#include <stdlib.h> 
#include <stdio.h>

#include "DSAM.h"

int MainSimulation(void)
{
	EarObjectPtr	stimulus = NULL, pEFilter = NULL, bMFilter = NULL;
	EarObjectPtr	hairCell = NULL;
	
	printf("Starting Test Harness...\n");
	
	/* Initialise EarObjects. */
	
	stimulus = Init_EarObject("Stim_click");
	pEFilter = Init_EarObject("Filt_BandPass");
	bMFilter = Init_EarObject("BM_GammaT");
	hairCell = Init_EarObject("IHC_Meddis86");

	/* Set up EarObject connections. */
	
	ConnectOutSignalToIn_EarObject( stimulus, pEFilter );
	ConnectOutSignalToIn_EarObject( pEFilter, bMFilter );
	ConnectOutSignalToIn_EarObject( bMFilter, hairCell );
 
	/* Initialise modules. */

	printf("Module parameters...\n\n" );

	ReadPars_ModuleMgr( stimulus, "Click1.par");
	PrintPars_ModuleMgr( stimulus );

	ReadPars_ModuleMgr( pEFilter, "PreEmph1.par");
	PrintPars_ModuleMgr( pEFilter );

	ReadPars_ModuleMgr( bMFilter, "GammaTLog.par");
	PrintPars_ModuleMgr( bMFilter );

	ReadPars_ModuleMgr( hairCell, "Meddis86.par");
	PrintPars_ModuleMgr( hairCell );


    /* Main simulation process */
	
	printf("\n");
	RunProcess_ModuleMgr(stimulus);
	RunProcess_ModuleMgr(pEFilter);
	RunProcess_ModuleMgr(bMFilter);
	RunProcess_ModuleMgr(hairCell);

	WriteOutSignal_DataFile("output.dat", hairCell);
	FreeAll_EarObject();
	printf("Finished test.\n");
	return(0);
	
}
