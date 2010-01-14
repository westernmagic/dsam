/**********************
 *
 * File:		PaUtilities.c
 * Purpose:		This module contains various useful utilities for parallel
 *				processing.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		18 Mar 1997
 * Updated:		
 * Copyright:	(c) 1997, 2010 Lowel P. O'Mard
 *
 *********************
 *
 *  This file is part of DSAM.
 *
 *  DSAM is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DSAM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DSAM.  If not, see <http://www.gnu.org/licenses/>.
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
	static WChar	*funcName = wxT("ProcessBatch");
	int		i, j, numBatches, batchOffset, numWorkChannels;

	numWorkChannels = DoFun(GetNumWorkers, audModel);
	numBatches = (int) ceil((double) numSettings / numWorkChannels);
	for (j = 0; j < numBatches; j++) {
		if (j == 0)
			DoFun6(QueueCommand, audModel, NULL, 0, PA_VOID, wxT("*"),
			  PRINT_PARS, ALL_WORKERS);
		batchOffset = j * numWorkChannels;
		for (i = 0; i < numWorkChannels; i++)
			if (!(* SetPars)((i + batchOffset) % numSettings)) {
				NotifyError(wxT("%s: Could not set parameters for worker[%d]."),
				  funcName, i);
				return(FALSE);
			}
		if (!DoProcess(audModel))
			return(FALSE);
		/* DoFun6(QueueCommand, audModel, NULL, 0, PA_VOID, wxT("*"),
		   WRITE_OUT_SIGNAL, ALL_WORKERS);
		DoFun(SendQueuedCommands, audModel); */
		for (i = 0; (i < numWorkChannels) && (i + batchOffset < numSettings);
		  i++)
			(* CalcResult)(audModel, i);
		DPrint(wxT("%s: Finished batch %d of %d...\n"), funcName, j + 1,
		  numBatches);
	}
	return(TRUE);

}
 
