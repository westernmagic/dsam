/**********************
 *
 * File:		PaUtilities.c
 * Purpose:		This module contains various useful utilities for parallel
 *				processing.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		18 Mar 1997
 * Updated:		
 * Copyright:	(c) 1997, University of Essex
 *
 **********************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DSAM_DO_NOT_DEFINE_MYAPP	1

#include "DSAM.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/


/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** ProcessBatch **********************************/

/*
 * This routine runs a process in parallel in batch fashion.
 */

BOOLN
ProcessBatch_MPI_Utility(EarObjectPtr audModel, BOOLN (* SetPars)(int),
  void (* CalcResult)(EarObjectPtr, int), int numSettings)
{
	static char	*funcName = "ProcessBatch";
	int		i, j, numBatches, batchOffset, numWorkChannels;

	numWorkChannels = DoFun(GetNumWorkers, audModel);
	numBatches = (int) ceil((double) numSettings / numWorkChannels);
	for (j = 0; j < numBatches; j++) {
		if (j == 0)
			DoFun6(QueueCommand, audModel, NULL, 0, PA_VOID, "*", PRINT_PARS,
			  ALL_WORKERS);
		batchOffset = j * numWorkChannels;
		for (i = 0; i < numWorkChannels; i++)
			if (!(* SetPars)((i + batchOffset) % numSettings)) {
				NotifyError("%s: Could not set parameters for worker[%d].",
				  funcName, i);
				return(FALSE);
			}
		if (!DoProcess(audModel))
			return(FALSE);
		/* DoFun6(QueueCommand, audModel, NULL, 0, PA_VOID, "*",
		   WRITE_OUT_SIGNAL, ALL_WORKERS);
		DoFun(SendQueuedCommands, audModel); */
		for (i = 0; (i < numWorkChannels) && (i + batchOffset < numSettings);
		  i++)
			(* CalcResult)(audModel, i);
		DPrint("%s: Finished batch %d of %d...\n", funcName, j + 1, numBatches);
	}
	return(TRUE);

}
 
