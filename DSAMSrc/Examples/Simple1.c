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
	
	stimulus = Init_EarObject("click");
	pEFilter = Init_EarObject("Filter_BandPass");
	bMFilter = Init_EarObject("BasilarM_GammaT");
	hairCell = Init_EarObject("IHC_Meddis86");

	/* Set up EarObject connections. */
	
	ConnectOutSignalToIn_EarObject( stimulus, pEFilter );
	ConnectOutSignalToIn_EarObject( pEFilter, bMFilter );
	ConnectOutSignalToIn_EarObject( bMFilter, hairCell );
 
	/* Initialise modules. */

	printf("Module parameters...\n\n" );

	DoFun1( ReadPars, stimulus, "Click1.par");
	DoFun( PrintPars, stimulus );

	DoFun1( ReadPars, pEFilter, "PreEmph1.par");
	DoFun( PrintPars, pEFilter );

	DoFun1( ReadPars, bMFilter, "GammaTLog.par");
	DoFun( PrintPars, bMFilter );

	DoFun1( ReadPars, hairCell, "Meddis86.par");
	DoFun( PrintPars, hairCell );


    /* Main simulation process */
	
	printf("\n");
	DoProcess(stimulus);
	RampUpOutSignal_Ramp(stimulus, Sine_Ramp, 2.5e-3);
	DoProcess(pEFilter);
	DoProcess(bMFilter);
	DoProcess(hairCell);

	WriteOutSignal_DataFile("output.dat", hairCell);
	FreeAll_EarObject();
	printf("Finished test.\n");
	return(0);
	
}
