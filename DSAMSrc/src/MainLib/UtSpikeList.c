/**********************
 *
 * File:		UtSpikeList.c
 * Purpose:		This utility module contains the routines for producing a spike
 *				train list from a signal.
 * Comments:	This implementation uses a linked list to record the spike
 *				times.  This list can be re-used, or it can retain the previous
 *				values.
 *				28-04-98 LPO: Correct segmented mode bug by adding the timeIndex
 *				value and also the riseDetected and lastValue arrays to the
 *				SpikeListSpec structure.
 * Author:		L. P. O'Mard
 * Created:		01 Apr 1996
 * Updated:		28 Apr 1998
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "UtSpikeList.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/**************************** Free ********************************************/

/*
 * This routine free's the spike specifications in the list.
 */

void
Free_SpikeList(SpikeSpecPtr *head)
{
	/* static const char *funcName = "Free_SpikeList"; */
	SpikeSpecPtr	p, temp;

	for (p = *head; p != NULL; ) {
		temp = p;
		p = p->next;
		free(temp);
	}
	*head = NULL;
}

/**************************** FreeListSpec ************************************/

/*
 * This routine free's the memory allocated for the spike list pointer to by
 * the 'head' pointers, and deallocates the 'head', 'tail' and 'current'
 * pointer arrays.
 */

void
FreeListSpec_SpikeList(SpikeListSpecPtr *p)
{
	int		i;

	if (*p == NULL)
		return;
	if ((*p)->head) {
		for (i = 0; i < (*p)->numChannels; i++)
			Free_SpikeList(&(*p)->head[i]);
		free((*p)->head);
	}
	if ((*p)->tail)
		free((*p)->tail);
	if ((*p)->current)
		free((*p)->current);
	if ((*p)->riseDetected)
		free((*p)->riseDetected);
	if ((*p)->lastValue)
		free((*p)->lastValue);
	free(*p);
	*p = NULL;

}

/**************************** InitListSpec ************************************/

/*
 * This intialises the head, tail and current pointers for a list.
 */

SpikeListSpecPtr
InitListSpec_SpikeList(int numChannels)
{
	static const char *funcName = "InitListSpec_SpikeList";
	BOOLN	ok = TRUE;
	SpikeListSpecPtr p;

	if (numChannels < 1) {
		NotifyError("%s: Illegal number of channels (%d).", funcName,
		  numChannels);
		return(NULL);
	}
	if ((p = (SpikeListSpecPtr) malloc(sizeof(SpikeListSpec))) == NULL) {
		NotifyError("%s: Out of memory SpikeListSpecPtr.", funcName);
		return(NULL);
	}
	p->numChannels = numChannels;
	if ((p->head = (SpikeSpecPtr *) calloc(p->numChannels,
	  sizeof(SpikeSpecPtr))) == NULL) {
		NotifyError("%s: Out of memory for %d 'head' pointers.", funcName,
		  p->numChannels);
		ok = FALSE;
	}
	if ((p->tail = (SpikeSpecPtr *) calloc(p->numChannels,
	  sizeof(SpikeSpecPtr))) == NULL) {
		NotifyError("%s: Out of memory for %d 'tail' pointers.", funcName,
		  p->numChannels);
		ok = FALSE;
	}
	if ((p->current = (SpikeSpecPtr *) calloc(p->numChannels,
	  sizeof(SpikeSpecPtr))) == NULL) {
		NotifyError("%s: Out of memory for %d 'current' pointers. ",
		  funcName, p->numChannels);
		ok = FALSE;
	}
	if ((p->riseDetected = (BOOLN *) calloc(p->numChannels, sizeof(BOOLN))) ==
	 NULL) {
		NotifyError("%s: Out of memory for the 'riseDetected[%d]' array. ",
		  funcName, p->numChannels);
		ok = FALSE;
	}
	if ((p->lastValue = (double *) calloc(p->numChannels, sizeof(double))) ==
	 NULL) {
		NotifyError("%s: Out of memory for 'lastValue[%d]' array. ",
		  funcName, p->numChannels);
		ok = FALSE;
	}
	p->timeIndex = PROCESS_START_TIME;
	if (!ok)
		FreeListSpec_SpikeList(&p);
	return(p);

}

/**************************** InsertSpikeSpec *********************************/

/*
 * This routine inserts a spike specification into the spike specification list
 * It will re-use the nodes in an existing list if one exists.
 * The list can be 'reset' by setting the 'current' pointer back to the 
 * head of the list.
 * The spike specification is always placed at the first node, if the list is
 * not initialised, or at the next node after the current position.
 * It returns FALSE if it fails in any way.
 * It assumes that the list specification has been correctly initialised,
 * And that there are the correct number of channels.
 */

BOOLN
InsertSpikeSpec_SpikeList(SpikeListSpecPtr listSpec, uShort channel,
  ChanLen timeIndex)
{
	static const char *funcName = "InsertSpikeSpec_SpikeSpec";
	SpikeSpecPtr	p;

	if ((listSpec->head[channel] == NULL) || (listSpec->current[channel] ==
	  NULL)) {
		if ((p = (SpikeSpecPtr) malloc(sizeof(SpikeSpec))) == NULL) {
			NotifyError("%s: Out of memory for spike specification.", funcName);
			return(FALSE);
		}
		if (listSpec->head[channel] == NULL) {
			listSpec->head[channel] = listSpec->tail[channel] = p;
			p->number = 1;
		} else {
			p->number = listSpec->tail[channel]->number + 1;
			listSpec->tail[channel] = listSpec->tail[channel]->next = p;
		}
		p->next = NULL;
		listSpec->current[channel] = NULL;
	} else {
		p = listSpec->current[channel];
		listSpec->current[channel] = listSpec->current[channel]->next;
	}
	p->timeIndex = timeIndex;
	return(TRUE);

}

/**************************** ResetListSpec ***********************************/

/*
 * This routine resets the lists in the list specification by setting the
 * current pointer back to the head of the list, it is not set to NULL.
 */

BOOLN
ResetListSpec_SpikeList(SpikeListSpecPtr listSpec)
{
	static const char *funcName = "ResetListSpec_SpikeList";
	int		i;

	if (listSpec == NULL) {
		NotifyError("%s: Attempt to reset NULL list specification.", funcName);
		return(FALSE);
	}
	for (i = 0; i < listSpec->numChannels; i++) {
		listSpec->current[i] = listSpec->head[i];
		listSpec->riseDetected[i] = FALSE;
		listSpec->lastValue[i] = 0.0;
	}
	listSpec->timeIndex = PROCESS_START_TIME;
	return(TRUE);

}

/**************************** GenerateList ************************************/

/*
 * This routine produces a spike list for each channel of a signal.
 * It assumes that the 'head' and 'current' pointer arrays have been correctly
 * initialised.
 * No allowances are made for flat-topped spikes.
 */

BOOLN
GenerateList_SpikeList(SpikeListSpecPtr listSpec, double eventThreshold,
  SignalDataPtr signal)
{
	static char	*funcName = "GenerateList_SpikeList";
	register	BOOLN		*riseDetected;
	register	ChanData	*inPtr, *lastValue;
	int		chan;
	ChanLen	i, startTime;

	if (listSpec == NULL) {
		NotifyError("%s: Attempt to use NULL list specification.", funcName);
		return(FALSE);
	}
	for (chan = 0; chan < signal->numChannels; chan++) {
		inPtr = signal->channel[chan];
		riseDetected = listSpec->riseDetected + chan;
		lastValue = listSpec->lastValue + chan;
		if (listSpec->timeIndex == PROCESS_START_TIME) {
			startTime = 1;
			*lastValue = *(inPtr++);
		} else
			startTime = 0;
		for (i = startTime; i < signal->length; i++) {
			if (!*riseDetected)
				*riseDetected = (*inPtr > *lastValue);
			else {
				if (*inPtr < *lastValue) {
					*riseDetected = FALSE;
					if (*lastValue > eventThreshold) {
						if (!InsertSpikeSpec_SpikeList(listSpec, chan,
						  listSpec->timeIndex + i - 1)) {
							NotifyError("%s: Could not insert spike "
							  "specification.", funcName);
							return(FALSE);
						}
					}
				}
			}
			*lastValue = *(inPtr++);
		}
	}
	listSpec->timeIndex += signal->length;
	return(TRUE);

}
