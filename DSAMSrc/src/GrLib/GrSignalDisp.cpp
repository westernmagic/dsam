/******************
 *		
 * File:		GrSignalDisp.cpp
 * Purpose: 	This graphics module serves as the interface between DSAM
 *				and the WxWin GUI.
 * Comments:	This module uses local parameters for the AveChannels utility
 *				module.
 *				The "ReadPars" routine for this module is different from the
 *				standard used by most of the other modules.
 *				21-10-97 LPO: Added absolute scaling mode.
 *				27-10-97 LPO: Added parameter flags so that only those
 *				parameters which have been set will be changed from their
 *				current/default values.
 *				27-10-97 LPO: Summary EarObject is only calculated when the
 *				summary display mode is on (default: off).
 *				12-11-97 LPO: Moved the SetProcessContinuity_.. call to before
 *				the setting of the display, otherwise the correct time is not
 *				shown in segmented mode (particularly for the second segment).
 *				26-11-97 LPO: Using new GeNSpecLists module for bool
 *				parameters.
 *				27-05-98 LPO: The default value of the summary display is now
 *				"OFF".
 *				28-06-98 LPO: Introduced the 'reduceChansInitialised' otherwise
 *				the Free_Utility_ReduceChannels(...) routine could be called
 *				with an undefined reduceChanPtr structure.
 *				30-06-98 LPO: The width parameter value is overridden when a
 *				signal with multiple windows is displayed.
 *				07-10-98 LPO: Changed some parameter names to correspond with
 *				the old AIM names.
 *				31-12-98 LPO: Problem with the frameDelay code causing a
 *				segmentation fault with egcs-1.1.1  What's the problem?
 * Author:		L.P.O'Mard
 * Created:		06 Jan 1995
 * Updated:		31 Dec 1998
 * Copyright:	(c) 1998, University of Essex
 *
 ******************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <time.h>

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
// For compilers that support precompilation, includes "wx.h".
#	include <wx/wxprec.h>

#	ifdef __BORLANDC__
	    #pragma hdrstop
#	endif

// Any files you want to include if not precompiling by including
// the whole of <wx/wx.h>
#	ifndef WX_PRECOMP
#		include <wx/wx.h>
#	endif

// Any files included regardless of precompiled headers
#endif

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GeNSpecLists.h"
#include "FiParFile.h"

#include "GrUtils.h"
#include "GrSimMgr.h"
#include "GrLine.h"
#include "GrSignalDisp.h"
#include "GrCanvas.h"
#include "GrDisplayS.h"

#ifndef DEBUG
#	define	DEBUG
#endif

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

SignalDispPtr	signalDispPtr = NULL;
int		signalDispPleaseLink = 0;

wxIcon		*sigDispIcon = NULL;

/******************************************************************************/
/*************************** NonGlobal declarations ***************************/
/******************************************************************************/

void	SetDisplay_SignalDisp(EarObjectPtr data);

/******************************************************************************/
/****************************** Subroutines & functions ***********************/
/******************************************************************************/

/****************************** PanelList *************************************/

/*
 * This routine returns the name specifier for the display panel list.
 */

NameSpecifier *
GetPanelList_SignalDisp(int index)
{
	static NameSpecifier	list[] = {

				{ "Signal",		DISPLAY_AUTOMATIC_SCALING },
				{ "Axis",		DISPLAY_X_DEC_PLACES },
				{ "General",	DISPLAY_FRAME_DELAY },
				{ "", 			DISPLAY_NULL }
			};
	;
	return(&list[index]);

}

/****************************** InitParNameList *******************************/

/*
 * This routine intialises the parameter list array.
 */

BOOLN
InitParNameList_SignalDisp(void)
{
	static NameSpecifier	list[] = {

					{ "AUTO_SCALING", DISPLAY_AUTOMATIC_SCALING },
					{ "CHANNEL_STEP", DISPLAY_CHANNEL_STEP },
					{ "MAGNIFICATION", DISPLAY_MAGNIFICATION },
					{ "MAXY", DISPLAY_MAX_Y },
					{ "MINY", DISPLAY_MIN_Y },
					{ "NUMGREYSCALES", DISPLAY_NUMGREYSCALES },
					{ "WIDTH", DISPLAY_WIDTH },
					{ "X_RESOLUTION", DISPLAY_X_RESOLUTION },
					{ "NORMALISATION", DISPLAY_Y_NORMALISATION_MODE },

					{ "X_DEC_PLACES", DISPLAY_X_DEC_PLACES },
					{ "X_TICKS", DISPLAY_X_TICKS },
					{ "Y_AXIS_MODE", DISPLAY_Y_AXIS_MODE },
					{ "Y_DEC_PLACES", DISPLAY_Y_DEC_PLACES },
					{ "Y_TICKS", DISPLAY_Y_TICKS },

					{ "FRAMEDELAY", DISPLAY_FRAME_DELAY },
					{ "MODE", DISPLAY_MODE },
					{ "SUMMARYDISPLAY", DISPLAY_SUMMARY_DISPLAY },
					{ "TOPMARGIN", DISPLAY_TOP_MARGIN },
					{ "WIN_HEIGHT", DISPLAY_WINDOW_HEIGHT },
					{ "WIN_TITLE", DISPLAY_WINDOW_TITLE },
					{ "WIN_WIDTH", DISPLAY_WINDOW_WIDTH },
					{ "WIN_X_POS", DISPLAY_WINDOW_X_POS },
					{ "WIN_Y_POS", DISPLAY_WINDOW_Y_POS },
					{ "", DISPLAY_NULL }
				};
	signalDispPtr->parNameList = list;
	return(TRUE);

}

/****************************** InitModeList **********************************/

/*
 * This routine intialises the mode list array.
 */

BOOLN
InitModeList_SignalDisp(void)
{
	static NameSpecifier	list[] = {

					{ "OFF",		GRAPH_MODE_OFF },
					{ "LINE",		GRAPH_MODE_LINE },
					{ "GREY_SCALE",	GRAPH_MODE_GREY_SCALE },
					{ "",			GRAPH_MODE_NULL }
				};
	signalDispPtr->modeList = list;
	return(TRUE);

}

/****************************** InitYAxisModeList *****************************/

/*
 * This routine intialises the y-axis mode list array.
 */

BOOLN
InitYAxisModeList_SignalDisp(void)
{
	static NameSpecifier	list[] = {

					{ "CHANNEL",		GRAPH_Y_AXIS_MODE_CHANNEL },
					{ "LINEAR_SCALE",	GRAPH_Y_AXIS_MODE_LINEAR_SCALE },
					{ "LOG_SCALE",		GRAPH_Y_AXIS_MODE_LOG_SCALE },
					{ "", 				GRAPH_Y_AXIS_MODE_NULL }
				};
	signalDispPtr->yAxisModeList = list;
	return(TRUE);

}

/**************************** Init ********************************************/

/*
 * This routine initialises the SignalDisp module parameters to default values.
 * The GLOBAL option is for hard programming - it sets the module's pointer to
 * the global parameter structure and initialises the parameters.
 * The LOCAL option is for generic programming - it initialises the parameter
 * structure currently pointed to by the module's parameter pointer.
 */

BOOLN
Init_SignalDisp(ParameterSpecifier parSpec)
{
	static const char *funcName = "Init_SignalDisp";

	if (parSpec == GLOBAL) {
		if (signalDispPtr != NULL)
			Free_SignalDisp();
		if ((signalDispPtr = (SignalDispPtr) malloc(sizeof(SignalDisp))) ==
		  NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName,
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (signalDispPtr == NULL) { 
			NotifyError("%s:  'local' pointer not set.", funcName, funcName);
			return(FALSE);
		}
	}
	
	signalDispPtr->automaticYScalingFlag = FALSE;
	signalDispPtr->yAxisModeFlag = FALSE;
	signalDispPtr->yNormalisationModeFlag = FALSE;
	signalDispPtr->modeFlag = FALSE;
	signalDispPtr->numGreyScalesFlag = FALSE;
	signalDispPtr->magnificationFlag = FALSE;
	signalDispPtr->summaryDisplayFlag = FALSE;
	signalDispPtr->xResolutionFlag = FALSE;
	signalDispPtr->minYFlag = FALSE;
	signalDispPtr->maxYFlag = FALSE;
	signalDispPtr->widthFlag = FALSE;
	signalDispPtr->frameDelayFlag = FALSE;
	signalDispPtr->frameHeightFlag = FALSE;
	signalDispPtr->frameWidthFlag = FALSE;
	signalDispPtr->frameXPosFlag = FALSE;
	signalDispPtr->frameYPosFlag = FALSE;
	signalDispPtr->titleFlag = FALSE;
	signalDispPtr->topMarginFlag = FALSE;
	signalDispPtr->channelStepFlag = FALSE;
	signalDispPtr->xTicksFlag = FALSE;
	signalDispPtr->xDecPlacesFlag = FALSE;
	signalDispPtr->yDecPlacesFlag = FALSE;
	signalDispPtr->yTicksFlag = FALSE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	signalDispPtr->parSpec = parSpec;
	signalDispPtr->automaticYScaling = GENERAL_BOOLEAN_ON;
	signalDispPtr->frameDelay = DISPLAY_DEFAULT_FRAME_DELAY;
	signalDispPtr->frameHeight = DISPLAY_DEFAULT_FRAME_HEIGHT;
	signalDispPtr->frameWidth = DISPLAY_DEFAULT_FRAME_WIDTH;
	wxGetApp().GetDefaultDisplayPos(&signalDispPtr->frameXPos,
	  &signalDispPtr->frameYPos);
	signalDispPtr->yAxisMode = GRAPH_Y_AXIS_MODE_CHANNEL;
	signalDispPtr->yNormalisationMode = LINE_YNORM_MIDDLE_MODE;
	signalDispPtr->mode = GRAPH_MODE_LINE;
	signalDispPtr->numGreyScales = GRAPH_NUM_GREY_SCALES;
	signalDispPtr->magnification = DEFAULT_SIGNAL_Y_SCALE;
	signalDispPtr->summaryDisplay = GENERAL_BOOLEAN_OFF;
	signalDispPtr->topMargin = GRAPH_TOP_MARGIN_PERCENT;
	signalDispPtr->width = DEFAULT_WIDTH;
	signalDispPtr->channelStep = DEFAULT_CHANNEL_STEP;
	signalDispPtr->xTicks = DEFAULT_X_TICKS;
	signalDispPtr->xDecPlaces = DEFAULT_X_DEC_PLACES;
	signalDispPtr->yDecPlaces = DEFAULT_Y_DEC_PLACES;
	signalDispPtr->yTicks = DEFAULT_Y_TICKS;
	signalDispPtr->title[0] = '\0';
	signalDispPtr->xResolution = DEFAULT_X_RESOLUTION;
	
	InitModeList_SignalDisp();
	InitYAxisModeList_SignalDisp();
	InitParNameList_SignalDisp();
	if (!SetUniParList_SignalDisp()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_SignalDisp();
		return(FALSE);
	}
	signalDispPtr->bufferCount = 0;
	signalDispPtr->buffer = NULL;
	signalDispPtr->summary = NULL;
	signalDispPtr->inLineProcess = FALSE;
	signalDispPtr->reduceChansInitialised = FALSE;
	signalDispPtr->registeredWithDisplayFlag = FALSE;
	signalDispPtr->displayCanDeleteFlag = FALSE;
	signalDispPtr->redrawFlag = FALSE;
	signalDispPtr->display = NULL;
	signalDispPtr->dialog = NULL;

	return(TRUE);

}

/********************************* Free ***************************************/

/*
 * This function releases of the memory allocated for the process variables.
 * It should be called when the module is no longer in use.
 * It is defined as returning a bool value because the generic module
 * interface requires that a non-void value be returned.
 * This routine will not free anything if the 'registeredWithDisplayFlag' flag
 * is set.
 */

BOOLN
Free_SignalDisp(void)
{
	if (signalDispPtr == NULL)
		return(TRUE);
	FreeProcessVariables_SignalDisp();
	signalDispPtr->displayCanDeleteFlag = TRUE;
	if (signalDispPtr->registeredWithDisplayFlag)
		return(TRUE);
	if (signalDispPtr->parList)
		FreeList_UniParMgr(&signalDispPtr->parList);
	if (signalDispPtr->parSpec == GLOBAL) {
		free(signalDispPtr);
		signalDispPtr = NULL;
	}
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */
 
BOOLN
SetUniParList_SignalDisp(void)
{
	static const char *funcName = "SetUniParList_SignalDisp";
	UniParPtr	pars;

	if ((signalDispPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  DISPLAY_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	SetGetPanelListFunc_UniParMgr(signalDispPtr->parList,
	  GetPanelList_SignalDisp);

	pars = signalDispPtr->parList->pars;
	SetPar_UniParMgr(&pars[DISPLAY_AUTOMATIC_SCALING],
	  signalDispPtr->parNameList[DISPLAY_AUTOMATIC_SCALING].name,
	  "Automatic scaling ('on' or 'off').",
	  UNIPAR_BOOL,
	  &signalDispPtr->automaticYScaling, NULL,
	  (void * (*)) SetAutomaticYScaling_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_CHANNEL_STEP], signalDispPtr->parNameList[
	  DISPLAY_CHANNEL_STEP].name,
	  "Channel stepping mode.",
	  UNIPAR_INT,
	  &signalDispPtr->channelStep, NULL,
	  (void * (*)) SetChannelStep_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_WINDOW_TITLE], signalDispPtr->parNameList[
	  DISPLAY_WINDOW_TITLE].name,
	  "Display window title.",
	  UNIPAR_STRING,
	  &signalDispPtr->title, NULL,
	  (void * (*)) SetTitle_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_FRAME_DELAY], signalDispPtr->parNameList[
	  DISPLAY_FRAME_DELAY].name,
	  "Delay between display frames (s)",
	  UNIPAR_REAL,
	  &signalDispPtr->frameDelay, NULL,
	  (void * (*)) SetFrameDelay_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_MAGNIFICATION],
	  signalDispPtr->parNameList[DISPLAY_MAGNIFICATION].name,
	  "Signal magnification.",
	  UNIPAR_REAL,
	  &signalDispPtr->magnification, NULL,
	  (void * (*)) SetMagnification_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_MAX_Y], signalDispPtr->parNameList[
	  DISPLAY_MAX_Y].name,
	  "Maximum Y value (for manual scaling).",
	  UNIPAR_REAL,
	  &signalDispPtr->maxY, NULL,
	  (void * (*)) SetMaxY_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_MIN_Y], signalDispPtr->parNameList[
	  DISPLAY_MIN_Y].name,
	  "Minimum Y Value (for manual scaling).",
	  UNIPAR_REAL,
	  &signalDispPtr->minY, NULL,
	  (void * (*)) SetMinY_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_MODE], signalDispPtr->parNameList[
	  DISPLAY_MODE].name,
	  "Display mode ('off', 'line' or 'gray_scale').",
	  UNIPAR_NAME_SPEC,
	  &signalDispPtr->mode, signalDispPtr->modeList,
	  (void * (*)) SetMode_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_NUMGREYSCALES], signalDispPtr->parNameList[
	  DISPLAY_NUMGREYSCALES].name,
	  "Number of grey scales.",
	  UNIPAR_INT,
	  &signalDispPtr->numGreyScales, NULL,
	  (void * (*)) SetNumGreyScales_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_SUMMARY_DISPLAY],signalDispPtr->parNameList[
	  DISPLAY_SUMMARY_DISPLAY].name,
	  "Summary display mode ('on' or 'off').",
	  UNIPAR_BOOL,
	  &signalDispPtr->summaryDisplay, NULL,
	  (void * (*)) SetSummaryDisplay_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_TOP_MARGIN], signalDispPtr->parNameList[
	  DISPLAY_TOP_MARGIN].name,
	  "Top margin for display (percent of display height).",
	  UNIPAR_REAL,
	  &signalDispPtr->topMargin, NULL,
	  (void * (*)) SetTopMargin_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_WIDTH], signalDispPtr->parNameList[
	  DISPLAY_WIDTH].name,
	  "Displayed signal width (seconds or x units).",
	  UNIPAR_REAL,
	  &signalDispPtr->width, NULL,
	  (void * (*)) SetWidth_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_WINDOW_HEIGHT], signalDispPtr->parNameList[
	  DISPLAY_WINDOW_HEIGHT].name,
	  "Display frame height (pixel units).",
	  UNIPAR_INT,
	  &signalDispPtr->frameHeight, NULL,
	  (void * (*)) SetFrameHeight_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_WINDOW_WIDTH], signalDispPtr->parNameList[
	  DISPLAY_WINDOW_WIDTH].name,
	  "Display frame width (pixel units).",
	  UNIPAR_INT,
	  &signalDispPtr->frameWidth, NULL,
	  (void * (*)) SetFrameWidth_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_WINDOW_X_POS], signalDispPtr->parNameList[
	  DISPLAY_WINDOW_X_POS].name,
	  "Display frame X position (pixel units).",
	  UNIPAR_INT,
	  &signalDispPtr->frameXPos, NULL,
	  (void * (*)) SetFrameXPos_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_WINDOW_Y_POS], signalDispPtr->parNameList[
	  DISPLAY_WINDOW_Y_POS].name,
	  "Display frame Y position (pixel units).",
	  UNIPAR_INT,
	  &signalDispPtr->frameYPos, NULL,
	  (void * (*)) SetFrameYPos_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_X_RESOLUTION], signalDispPtr->parNameList[
	  DISPLAY_X_RESOLUTION].name,
	  "Resolution of X scale (1 - low, fractions are higher).",
	  UNIPAR_REAL,
	  &signalDispPtr->xResolution, NULL,
	  (void * (*)) SetXResolution_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_Y_NORMALISATION_MODE],
	  signalDispPtr->parNameList[DISPLAY_Y_NORMALISATION_MODE].name,
	  "Y normalisation mode ('bottom' or 'middle').",
	  UNIPAR_NAME_SPEC,
	  &signalDispPtr->yNormalisationMode, yNormModeList_Line,
	  (void * (*)) SetYNormalisationMode_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_Y_AXIS_MODE],
	  signalDispPtr->parNameList[DISPLAY_Y_AXIS_MODE].name,
	  "Y axis mode ('channel' (No.) or 'scale').",
	  UNIPAR_NAME_SPEC,
	  &signalDispPtr->yAxisMode, signalDispPtr->yAxisModeList,
	  (void * (*)) SetYAxisMode_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_X_TICKS],
	  signalDispPtr->parNameList[DISPLAY_X_TICKS].name,
	  "X axis tick marks.",
	  UNIPAR_INT,
	  &signalDispPtr->xTicks, NULL,
	  (void * (*)) SetXTicks_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_X_DEC_PLACES],
	  signalDispPtr->parNameList[DISPLAY_X_DEC_PLACES].name,
	  "X axis scale decimal places.",
	  UNIPAR_INT,
	  &signalDispPtr->xDecPlaces, NULL,
	  (void * (*)) SetXDecPlaces_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_Y_DEC_PLACES],
	  signalDispPtr->parNameList[DISPLAY_Y_DEC_PLACES].name,
	  "Y axis scale decimal places.",
	  UNIPAR_INT,
	  &signalDispPtr->yDecPlaces, NULL,
	  (void * (*)) SetYDecPlaces_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_Y_TICKS],
	  signalDispPtr->parNameList[DISPLAY_Y_TICKS].name,
	  "Y axis tick marks.",
	  UNIPAR_INT,
	  &signalDispPtr->yTicks, NULL,
	  (void * (*)) SetYTicks_SignalDisp);

	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_SignalDisp(void)
{
	static const char *funcName = "GetUniParListPtr_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(NULL);
	}
	if (signalDispPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(signalDispPtr->parList);

}

/**************************** SetAutomaticYScaling ****************************/

/*
 * This routine sets the automatic scaling mode for the display.
 * If set to true, the scaling will be direct and related only to the
 * signal yScale parameter.
 */

BOOLN
SetAutomaticYScaling_SignalDisp(char *theAutomaticYScaling)
{
	static const char *funcName = "SetAutomaticYScaling_SignalDisp";
	int		specifier;

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theAutomaticYScaling,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal switch state (%s).", funcName,
		  theAutomaticYScaling);
		return(FALSE);
	}
	signalDispPtr->automaticYScaling = specifier;
	signalDispPtr->automaticYScalingFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	if (!signalDispPtr->automaticYScaling) {
		signalDispPtr->parList->pars[DISPLAY_MAX_Y].enabled = TRUE;
		signalDispPtr->parList->pars[DISPLAY_MIN_Y].enabled = TRUE;
	} else {
		signalDispPtr->parList->pars[DISPLAY_MAX_Y].enabled = FALSE;
		signalDispPtr->parList->pars[DISPLAY_MIN_Y].enabled = FALSE;
	}
	signalDispPtr->parList->updateFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetFrameDelay ***********************************/

/*
 * This routine sets the frame delay for the display.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetFrameDelay_SignalDisp(double frameDelay)
{
	static const char *funcName = "SetFrameDelay_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	signalDispPtr->frameDelay = frameDelay;
	signalDispPtr->frameDelayFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetChannelStep **********************************/

/*
 * This routine sets the chennl step for the display.
 * This is a boolean response, hence only non-zero values are TRUE.
 */

BOOLN
SetChannelStep_SignalDisp(int channelStep)
{
	static const char *funcName = "SetChannelStep_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	signalDispPtr->channelStep = channelStep;
	signalDispPtr->channelStepFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetFrameHeight **********************************/

/*
 * This routine sets the frame height for the display.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetFrameHeight_SignalDisp(int frameHeight)
{
	static const char *funcName = "SetFrameHeight_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (frameHeight <= 0) {
		NotifyError("%s: Value must be greater than zero (%g)", funcName,
		  frameHeight);
		return(FALSE);
	}
	signalDispPtr->frameHeight = frameHeight;
	signalDispPtr->frameHeightFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetFrameWidth ***********************************/

/*
 * This routine sets the frame width for the display.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetFrameWidth_SignalDisp(int frameWidth)
{
	static const char *funcName = "SetFrameWidth_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (frameWidth <= 0) {
		NotifyError("%s: Value must be greater than zero (%g)", funcName,
		  frameWidth);
		return(FALSE);
	}
	signalDispPtr->frameWidth = frameWidth;
	signalDispPtr->frameWidthFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetFrameXPos ************************************/

/*
 * This routine sets the frame x position for the display.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetFrameXPos_SignalDisp(int frameXPos)
{
	static const char *funcName = "SetFrameXPos_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	signalDispPtr->frameXPos = frameXPos;
	signalDispPtr->frameXPosFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetFrameYPos ************************************/

/*
 * This routine sets the frame x position for the display.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetFrameYPos_SignalDisp(int frameYPos)
{
	static const char *funcName = "SetFrameYPos_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	signalDispPtr->frameYPos = frameYPos;
	signalDispPtr->frameYPosFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetYAxisMode ************************************/

/*
 * This routine sets the module's y-axis mode.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetYAxisMode_SignalDisp(char *theYAxisMode)
{
	static const char *funcName = "SetYAxisMode_SignalDisp";
	int		specifier;

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theYAxisMode,
	  signalDispPtr->yAxisModeList)) == GRAPH_MODE_NULL) {
		NotifyError("%s: Illegal mode name (%s).", funcName, theYAxisMode);
		return(FALSE);
	}
	signalDispPtr->yAxisMode = specifier;
	signalDispPtr->yAxisModeFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetYNormalisationMode ***************************/

/*
 * This routine sets the automatic y offset mode for the display.
 * If set to true, the scaling will be direct and related only to the
 * signal yScale parameter.
 */

BOOLN
SetYNormalisationMode_SignalDisp(char *theYNormalisationMode)
{
	static const char *funcName = "SetYNormalisationMode_SignalDisp";
	int		specifier;

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theYNormalisationMode,
	  yNormModeList_Line)) == LINE_YNORM_MODE_NULL) {
		NotifyError("%s: Illegal switch state (%s).", funcName,
		  theYNormalisationMode);
		return(FALSE);
	}
	signalDispPtr->yNormalisationMode = specifier;
	signalDispPtr->yNormalisationModeFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetXResolution **********************************/

/*
 * This routine sets the x resolution for the display.
 * The x resolution is used in the algorithm for optimising printing x values.
 * It prevents inefficient "over-plotting" of points.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetXResolution_SignalDisp(double xResolution)
{
	static const char *funcName = "SetXResolution_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((xResolution < FLT_EPSILON) || (xResolution > 1.0)) {
		NotifyError("%s: Value must be in range %.2g - 1.0 (%g)", funcName,
		  FLT_EPSILON, xResolution);
		return(FALSE);
	}
	signalDispPtr->xResolution = xResolution;
	signalDispPtr->xResolutionFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetMagnification ********************************/

/*
 * This routine sets the signal y scale for printing.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetMagnification_SignalDisp(double magnification)
{
	static const char *funcName = "SetMagnification_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (magnification < FLT_EPSILON) {
		NotifyError("%s: Illegal value (%g)", funcName, magnification);
		return(FALSE);
	}
	signalDispPtr->magnification = magnification;
	signalDispPtr->magnificationFlag = TRUE;;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetMaxY *****************************************/

/*
 * This routine sets the maximum Y scale for use when that automatic scaling is
 * disabled.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetMaxY_SignalDisp(double maxY)
{
	static const char *funcName = "SetMaxY_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	signalDispPtr->maxY = maxY;
	signalDispPtr->maxYFlag = TRUE;;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetMinY *****************************************/

/*
 * This routine sets the minimum Y scale for use when that automatic scaling is
 * disabled.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetMinY_SignalDisp(double minY)
{
	static const char *funcName = "SetMinY_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	signalDispPtr->minY = minY;
	signalDispPtr->minYFlag = TRUE;;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetTopMargin ************************************/

/*
 * This routine sets the percentage of the display height to use as the top
 * margin.
 */

BOOLN
SetTopMargin_SignalDisp(double topMargin)
{
	static const char *funcName = "SetTopMargin_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	signalDispPtr->topMargin = topMargin;
	signalDispPtr->topMarginFlag = TRUE;;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetWidth ****************************************/

/*
 * This routine sets the width for the display.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetWidth_SignalDisp(double width)
{
	static const char *funcName = "SetWidth_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	signalDispPtr->width = width;
	signalDispPtr->widthFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetMode *****************************************/

/*
 * This routine sets the module's display mode.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetMode_SignalDisp(char *theMode)
{
	static const char *funcName = "SetMode_SignalDisp";
	int		specifier;

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode,
	  signalDispPtr->modeList)) == GRAPH_MODE_NULL) {
		NotifyError("%s: Illegal mode name (%s).", funcName, theMode);
		return(FALSE);
	}
	signalDispPtr->mode = specifier;
	signalDispPtr->modeFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetNumGreyScales ********************************/

/*
 * This routine sets the number of grey scales in the 'grey_scale' display mode.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetNumGreyScales_SignalDisp(int theNumGreyScales)
{
	static const char *funcName = "SetNumGreyScales_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theNumGreyScales < 1) {
		NotifyError("%s: Illegal value (%d)", funcName, theNumGreyScales);
		return(FALSE);
	}
	signalDispPtr->numGreyScales = theNumGreyScales;
	signalDispPtr->numGreyScalesFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetSummaryDisplay *******************************/

/*
 * This routine sets the summary display mode for the display.
 * If set to true, the scaling will be direct and related only to the
 * signal yScale parameter.
 */

BOOLN
SetSummaryDisplay_SignalDisp(char *theSummaryDisplay)
{
	static const char *funcName = "SetSummaryDisplay_SignalDisp";
	int		specifier;

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theSummaryDisplay,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal switch state (%s).", funcName,
		  theSummaryDisplay);
		return(FALSE);
	}
	signalDispPtr->summaryDisplayFlag = TRUE;
	signalDispPtr->summaryDisplay = specifier;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetTitle ****************************************/

/*
 * This routine sets the 'title' field of the 'signalDisp' structure.
 */

BOOLN
SetTitle_SignalDisp(char *title)
{
	static const char *funcName = "SetTitle_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (*title == (char) NULL) {
		NotifyError("%s: Illegal window title.", funcName);
		return(FALSE);
	}
	signalDispPtr->titleFlag = TRUE;
	strcpy(signalDispPtr->title, title);
	if (signalDispPtr->display)
		signalDispPtr->display->SetTitle(title);
	return(TRUE);

}

/**************************** SetXTicks ***************************************/

/*
 * This routine sets the X axis ticks for the display.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetXTicks_SignalDisp(int xTicks)
{
	static const char *funcName = "SetXTicks_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (xTicks <= 0) {
		NotifyError("%s: Value must be greater than zero (%g)", funcName,
		  xTicks);
		return(FALSE);
	}
	signalDispPtr->xTicks = xTicks;
	signalDispPtr->xTicksFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetXDecPlaces ***********************************/

/*
 * This routine sets the decimal places displayed for the display x-axis.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetXDecPlaces_SignalDisp(int xDecPlaces)
{
	static const char *funcName = "SetXDecPlaces_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (xDecPlaces < 0) {
		NotifyError("%s: Value cannot be negative (%g)", funcName, xDecPlaces);
		return(FALSE);
	}
	signalDispPtr->xDecPlaces = xDecPlaces;
	signalDispPtr->xDecPlacesFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetYDecPlaces ***********************************/

/*
 * This routine sets the decimal places displayed for the display x-axis.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetYDecPlaces_SignalDisp(int yDecPlaces)
{
	static const char *funcName = "SetYDecPlaces_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (yDecPlaces < 0) {
		NotifyError("%s: Value cannot be negative (%g)", funcName, yDecPlaces);
		return(FALSE);
	}
	signalDispPtr->yDecPlaces = yDecPlaces;
	signalDispPtr->yDecPlacesFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetYTicks ***************************************/

/*
 * This routine sets the Y axis ticks for the display.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetYTicks_SignalDisp(int yTicks)
{
	static const char *funcName = "SetYTicks_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (yTicks <= 0) {
		NotifyError("%s: Value must be greater than zero (%g)", funcName,
		  yTicks);
		return(FALSE);
	}
	signalDispPtr->yTicks = yTicks;
	signalDispPtr->yTicksFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/********************************* CheckPars **********************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised.
 * It returns TRUE if there are no problems.
 */

BOOLN
CheckPars_SignalDisp(void)
{
	static const char *funcName = "CheckPars_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	return(TRUE);
	
}	

/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetPars_SignalDisp(char *theMode, char *theAutomaticYScaling,
  char *theYNormalisationMode, char *theYAxisMode, char *theSummaryDisplay,
  char *theTitle, int theChannelStep, int theNumGreyScales, int theFrameHeight,
  int theFrameWidth, int theFrameXPos, int theFrameYPos, int theXDecPlaces,
  int theXTicks, int theYDecPlaces, int theYTicks, double theFrameDelay,
  double theMagnification, double theXResolution, double theMaxY,
  double theMinY, double theTopMargin, double theWidth)
{
	bool	ok;
	
	ok = TRUE;
	if (!SetAutomaticYScaling_SignalDisp(theAutomaticYScaling))
		ok = FALSE;
	if (!SetChannelStep_SignalDisp(theChannelStep))
		ok = FALSE;
	if (!SetFrameDelay_SignalDisp(theFrameDelay))
		ok = FALSE;
	if (!SetFrameHeight_SignalDisp(theFrameHeight))
		ok = FALSE;
	if (!SetFrameWidth_SignalDisp(theFrameWidth))
		ok = FALSE;
	if (!SetFrameXPos_SignalDisp(theFrameXPos))
		ok = FALSE;
	if (!SetFrameYPos_SignalDisp(theFrameYPos))
		ok = FALSE;
	if (!SetYAxisMode_SignalDisp(theYAxisMode))
		ok = FALSE;
	if (!SetYNormalisationMode_SignalDisp(theYNormalisationMode))
		ok = FALSE;
	if (!SetMode_SignalDisp(theMode))
		ok = FALSE;
	if (!SetNumGreyScales_SignalDisp(theNumGreyScales))
		ok = FALSE;
	if (!SetMagnification_SignalDisp(theMagnification))
		ok = FALSE;
	if (!SetSummaryDisplay_SignalDisp(theSummaryDisplay))
		ok = FALSE;
	if (!SetXResolution_SignalDisp(theXResolution))
		ok = FALSE;
	if (!SetMaxY_SignalDisp(theMaxY))
		ok = FALSE;
	if (!SetMinY_SignalDisp(theMinY))
		ok = FALSE;
	if (!SetTitle_SignalDisp(theTitle))
		ok = FALSE;
	if (!SetTopMargin_SignalDisp(theTopMargin))
		ok = FALSE;
	if (!SetWidth_SignalDisp(theWidth))
		ok = FALSE;
	if (!SetXTicks_SignalDisp(theXTicks))
		ok = FALSE;
	if (!SetXDecPlaces_SignalDisp(theXDecPlaces))
		ok = FALSE;
	if (!SetYDecPlaces_SignalDisp(theYDecPlaces))
		ok = FALSE;
	if (!SetYTicks_SignalDisp(theYTicks))
		ok = FALSE;
	return(ok);
	
}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */
 
BOOLN
PrintPars_SignalDisp(void)
{
	static const char *funcName = "PrintPars_SignalDisp";

	if (!CheckPars_SignalDisp()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("SignalDisp Module Parameters:-\n");
	DPrint("\tWindow title = %s,\n", signalDispPtr->title);
	DPrint("\tMode = %s,\n",
	  signalDispPtr->modeList[signalDispPtr->mode].name);
	if (signalDispPtr->mode == GRAPH_MODE_GREY_SCALE)
		DPrint("\tNo. grey scales = %d.\n", signalDispPtr->numGreyScales);
	DPrint("\tAutomatic scaling mode: %s\n",
	  BooleanList_NSpecLists(signalDispPtr->automaticYScaling)->name);
	DPrint("\tChannel step for display: %d\n", signalDispPtr->channelStep);
	if (!signalDispPtr->automaticYScaling)
		DPrint("\tMinimum/Maximum Y values: %g/%g units.\n",
		  signalDispPtr->minY, signalDispPtr->maxY);
	DPrint("\tY normalisation mode: %s\n",
	  yNormModeList_Line[signalDispPtr->yNormalisationMode].name);
	DPrint("\tSummary display mode: %s\n",
	  BooleanList_NSpecLists(signalDispPtr->summaryDisplay)->name);
	DPrint("\tSignal Y scale = %g.\n", signalDispPtr->magnification);
	DPrint("\tX resolution = %g (<= 1.0)\n", signalDispPtr->xResolution);
	DPrint("\tX axis ticks = %d\n", signalDispPtr->xTicks);
	DPrint("\tX axis scale decimal places = %d\n", signalDispPtr->xDecPlaces);
	DPrint("\tY-axis mode = %s,\n",
	  signalDispPtr->yAxisModeList[signalDispPtr->yAxisMode].name);
	DPrint("\tY axis scale decimal places = %d\n", signalDispPtr->yDecPlaces);
	DPrint("\tY axis ticks = %d\n", signalDispPtr->yTicks);
	DPrint("\tTop margin percentage = %g %%\n", signalDispPtr->topMargin);
	DPrint("\tWidth = ");
	if (signalDispPtr->width < 0.0)
		DPrint("prev. signal length\n");
	else
		DPrint("%g ms\n", MILLI(signalDispPtr->width));
	DPrint("\tFrame: dimensions %d x %d\t", signalDispPtr->frameWidth,
	  signalDispPtr->frameHeight);
	DPrint("\tFrame: position %d x %d\t", signalDispPtr->frameXPos,
	  signalDispPtr->frameYPos);
	DPrint("\tDelay %g ms\n", MILLI(signalDispPtr->frameDelay));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 * This routine is different from the standard "ReadPars" routines which are
 * in most of the other modules: (1) As all parameters have a default value,
 * a "NO_FILE" parameter file name will just return TRUE; (2) parameter lines
 * are read in as required - there is no standard parameter file format.
 */
 
BOOLN
ReadPars_SignalDisp(char *parFileName)
{
	static const char *funcName = "ReadPars_SignalDisp";
	bool	ok;
	char	*parFilePath;
	char	parName[MAXLINE], parValue[MAXLINE];
	FILE    *fp;
    
    if (strcmp(parFileName, NO_FILE) == 0)
    	return(TRUE);
	parFilePath = GetParsFileFPath_Common(parFileName);
    if ((fp = fopen(parFilePath, "r")) == NULL) {
        NotifyError("%s: Cannot open data file '%s'.\n", funcName, parFilePath);
		return(FALSE);
    }
    DPrint("%s: Reading from '%s':\n", funcName, parFilePath);
    Init_ParFile();
	ok = TRUE;
	SetEmptyLineMessage_ParFile(FALSE);
	while (GetPars_ParFile(fp, "%s %s", parName, parValue))
		switch (Identify_NameSpecifier(parName, signalDispPtr->parNameList)) {
		case DISPLAY_AUTOMATIC_SCALING:
			if (!SetAutomaticYScaling_SignalDisp(parValue))
				ok = FALSE;
			break;
		case DISPLAY_CHANNEL_STEP:
			if (!SetChannelStep_SignalDisp(atoi(parValue)))
				ok = FALSE;
			break;
		case DISPLAY_FRAME_DELAY:
			if (!SetFrameDelay_SignalDisp(atof(parValue)))
				ok = FALSE;
			break;
		case DISPLAY_WINDOW_HEIGHT:
			if (!SetFrameHeight_SignalDisp(atoi(parValue)))
				ok = FALSE;
			break;
		case DISPLAY_WINDOW_TITLE:
			if (!SetTitle_SignalDisp(parValue))
				ok = FALSE;
			break;
		case DISPLAY_WINDOW_WIDTH:
			if (!SetFrameWidth_SignalDisp(atoi(parValue)))
				ok = FALSE;
			break;
		case DISPLAY_WINDOW_X_POS:
			if (!SetFrameXPos_SignalDisp(atoi(parValue)))
				ok = FALSE;
			break;
		case DISPLAY_WINDOW_Y_POS:
			if (!SetFrameYPos_SignalDisp(atoi(parValue)))
				ok = FALSE;
			break;
		case DISPLAY_Y_NORMALISATION_MODE:
			if (!SetYNormalisationMode_SignalDisp(parValue))
				ok = FALSE;
			break;
		case DISPLAY_MAX_Y:
			if (!SetMaxY_SignalDisp(atof(parValue)))
				ok = FALSE;
			break;
		case DISPLAY_MIN_Y:
			if (!SetMinY_SignalDisp(atof(parValue)))
				ok = FALSE;
			break;
		case DISPLAY_MODE:
			if (!SetMode_SignalDisp(parValue))
				ok = FALSE;
			break;
		case DISPLAY_NUMGREYSCALES:
			if (!SetNumGreyScales_SignalDisp(atoi(parValue)))
				ok = FALSE;
			break;
		case DISPLAY_TOP_MARGIN:
			if (!SetTopMargin_SignalDisp(atof(parValue)))
				ok = FALSE;
			break;
		case DISPLAY_WIDTH:
			if (!SetWidth_SignalDisp(atof(parValue)))
				ok = FALSE;
			break;
		case DISPLAY_MAGNIFICATION:
			if (!SetMagnification_SignalDisp(atof(parValue)))
				ok = FALSE;
			break;
		case DISPLAY_SUMMARY_DISPLAY:
			if (!SetSummaryDisplay_SignalDisp(parValue))
				ok = FALSE;
			break;
		case DISPLAY_X_RESOLUTION:
			if (!SetXResolution_SignalDisp(atof(parValue)))
				ok = FALSE;
			break;
		case DISPLAY_X_DEC_PLACES:
			if (!SetXDecPlaces_SignalDisp(atoi(parValue)))
				ok = FALSE;
			break;
		case DISPLAY_X_TICKS:
			if (!SetXTicks_SignalDisp(atoi(parValue)))
				ok = FALSE;
			break;
		case DISPLAY_Y_AXIS_MODE:
			if (!SetYAxisMode_SignalDisp(parValue))
				ok = FALSE;
			break;
		case DISPLAY_Y_DEC_PLACES:
			if (!SetYDecPlaces_SignalDisp(atoi(parValue)))
				ok = FALSE;
			break;
		case DISPLAY_Y_TICKS:
			if (!SetYTicks_SignalDisp(atoi(parValue)))
				ok = FALSE;
			break;
	case DISPLAY_NULL:
			NotifyError("%s: Illegal parameter name (%s).", funcName, parName);
			ok = FALSE;
			break;
		} /* case */
	SetEmptyLineMessage_ParFile(TRUE);
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s:  Invalid parameters, in module parameter file '%s'.",
		  funcName, parFilePath);
		return(FALSE);
	}
	return(TRUE);
    
}

/**************************** SetProcessMode **********************************/

/*
 * This routine checks whether this module is being used in-line within an 
 * EarObject pipline.
 * This is checked by ensuring that this is the first 
 * module to use the EarObject, or that it was previously used by this module,
 * i.e. the process name has not been set, or it is set to this module's name.
 * This routine assumes that 'data' has been correctly initialised.
 */

BOOLN
SetProcessMode_SignalDisp(EarObjectPtr data)
{
	static const char *funcName = "SetProcessMode_SignalDisp";

	if ((data->processName == NULL) || (strncmp(data->processName,
		  DISPLAY_PROCESS_NAME, strlen(DISPLAY_PROCESS_NAME)) == 0) ||
		  (strcmp(data->processName, NULL_MODULE_PROCESS_NAME) == 0)) {
		SetProcessName_EarObject(data, "%s (%d)", DISPLAY_PROCESS_NAME,
		  (int) data->handle);
		if (data->outSignal != NULL)
			if (!SameType_SignalData_NoDiagnostics(data->inSignal[0],
			  data->outSignal))
				data->updateCustomersFlag = TRUE;
		if (data->outSignal != data->inSignal[0])
			data->updateCustomersFlag = TRUE;
		data->outSignal = data->inSignal[0];
		signalDispPtr->inLineProcess = TRUE;
	} else {
		if (!CheckPars_SignalData(data->outSignal)) {
			NotifyError("%s: Output signal not correctly set for 'sample' use.",
			  funcName);
			return(FALSE);
		}
		signalDispPtr->inLineProcess = FALSE;
	}
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_SignalDisp(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_SignalDisp";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	signalDispPtr = (SignalDispPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_SignalDisp(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_SignalDisp";

	if (!SetParsPointer_SignalDisp(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_SignalDisp(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = signalDispPtr;
	theModule->CheckPars = CheckPars_SignalDisp;
	theModule->Free = Free_SignalDisp;
	theModule->GetUniParListPtr = GetUniParListPtr_SignalDisp;
	theModule->PrintPars = PrintPars_SignalDisp;
	theModule->ReadPars = ReadPars_SignalDisp;
	theModule->RunProcess = ShowSignal_SignalDisp;
	theModule->SetParsPointer = SetParsPointer_SignalDisp;
	return(TRUE);

}

/****************************** CheckData *************************************/

/*
 * This routine checks that the 'data' EarObject and input signal are
 * correctly initialised.
 * It should also include checks that ensure that the module's
 * parameters are compatible with the signal parameters, i.e. dt is
 * not too small, etc...
 * The 'CheckRamp_SignalData()' can be used instead of the
 * 'CheckInit_SignalData()' routine if the signal must be ramped for
 * the process.
 */

BOOLN
CheckData_SignalDisp(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_SignalDisp";

	if ((signalDispPtr->width > 0.0) && ((ChanLen) floor(signalDispPtr->width /
	  data->outSignal->dt + 0.5) < (data->outSignal->length /
	  data->outSignal->numWindowFrames))) {
		NotifyError("%s: Width window (%g ms) must not be less than the "
		  "signal duration (%g ms).", funcName, MILLI(signalDispPtr->width),
		  MILLI(_GetDuration_SignalData(data->outSignal)));
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** CheckForDisplay *********************************/

/*
 * This routine checks to see if a display already exists for the EarObject.
 * It will then delete the current signalDispPtr structure and replace it with
 * the stored pointer.
 * It checks to ensure that the current signalDispPtr does not point to a valid
 * existing structure, in which case it does nothing.
 */

void
CheckForDisplay_SignalDisp(long handle)
{
	wxNode	*myNode;
	DisplayS	*display;

	if ((myNode = myChildren.Find(handle)) == NULL)
		return;
	display = (DisplayS *) myNode->Data();
	if (signalDispPtr == display->GetSignalDispPtr())
		return;
	Free_SignalDisp();
	signalDispPtr = display->GetSignalDispPtr();

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It initialises the summary EarObject used in the display module.
 * It assumes that the centre frequency list is set up correctly.
 * It also assumes that the output signal has already been initialised.
 */

BOOLN
InitProcessVariables_SignalDisp(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_SignalDisp";
	SignalDataPtr	signal, buffer;
	
	if (signalDispPtr->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		signal = data->outSignal;
		if (signalDispPtr->updateProcessVariablesFlag ||
		  data->updateProcessFlag) {
			FreeProcessVariables_SignalDisp();
			CheckForDisplay_SignalDisp((long) data->handle);
			if ((signalDispPtr->summary = Init_EarObject("NULL")) == NULL) {
				NotifyError("%s: Out of memory for summary EarObject.",
				  funcName);
				return(FALSE);
			}
			if ((signalDispPtr->width > 0.0) || (signal->numWindowFrames !=
			  SIGNALDATA_DEFAULT_NUM_WINDOW_FRAMES)) {
				if ((signalDispPtr->buffer = Init_EarObject("NULL")) == NULL) {
					NotifyError("%s: Out of memory for buffer EarObject.",
					  funcName);
					return(FALSE);
				}
				if (signal->numWindowFrames !=
				  SIGNALDATA_DEFAULT_NUM_WINDOW_FRAMES)
					signalDispPtr->width = _GetDuration_SignalData(signal) /
					  signal->numWindowFrames;
			}
			reduceChansPtr = &signalDispPtr->reduceChans;
			Init_Utility_ReduceChannels(LOCAL);
			signalDispPtr->reduceChansInitialised = TRUE;
			signalDispPtr->parList->updateFlag = TRUE;
			signalDispPtr->updateProcessVariablesFlag = FALSE;
			data->updateProcessFlag = FALSE;
		}
		signalDispPtr->bufferCount = 0;
		if (signalDispPtr->buffer) {
			if (!InitOutSignal_EarObject(signalDispPtr->buffer,
			  signal->numChannels, (ChanLen) floor(signalDispPtr->width /
			  signal->dt + 0.5), signal->dt)) {
				NotifyError("%s: Could not initialise buffer signal.",
				  funcName);
				return(FALSE);
			}
			buffer = signalDispPtr->buffer->outSignal;
			SetInterleaveLevel_SignalData(buffer, signal->interleaveLevel);
			SetNumWindowFrames_SignalData(buffer, signal->numWindowFrames);
			SetOutputTimeOffset_SignalData(buffer, signal->outputTimeOffset);
			SetStaticTimeFlag_SignalData(buffer, signal->staticTimeFlag);
			CopyInfo_SignalData(buffer, signal);
		}
		/**** Put things that need to be initialised at start time here. ***/
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 * It just checks the first set of coefficients, and assumes that the others
 * are in the same state.
 */

BOOLN
FreeProcessVariables_SignalDisp(void)
{

 	Free_EarObject(&signalDispPtr->buffer);
 	Free_EarObject(&signalDispPtr->summary);
		
	if (signalDispPtr->reduceChansInitialised) {
 		reduceChansPtr = &signalDispPtr->reduceChans;
 		Free_Utility_ReduceChannels();
 		reduceChansPtr = NULL;
	}
 	signalDispPtr->updateProcessVariablesFlag = TRUE;
 	return(TRUE);

}

/**************************** ProcessBuffer ***********************************/

/*
 * This routine processes the display buffer.
 * It reads in data into the buffer, shifting existing data along if necessary.
 */

void
ProcessBuffer_SignalDisp(SignalDataPtr signal, EarObjectPtr bufferEarObj,
  int windowFrame)
{
	/* static const char *funcName = "ProcessBuffer_SignalDisp"; */
	register	ChanData	*inPtr, *outPtr;
	int		chan;
	ChanLen	j, shift, signalLength;
	SignalDataPtr buffer;

	buffer = bufferEarObj->outSignal;
	signalLength = signal->length / signal->numWindowFrames;
	if (signalDispPtr->bufferCount + signalLength > buffer->length) {
		shift = signalLength - (buffer->length - signalDispPtr->bufferCount);
		if (shift < buffer->length)
			for (chan = 0; chan < signal->numChannels; chan++) {
				inPtr = buffer->channel[chan] + shift;
				outPtr = buffer->channel[chan];
				for (j = 0; j < signalDispPtr->bufferCount; j++)
					*outPtr++ = *inPtr++;
			}
		signalDispPtr->bufferCount -= shift;
		if (!signal->staticTimeFlag)
			buffer->outputTimeOffset += shift * buffer->dt;
	}
	for (chan = 0; chan < signal->numChannels; chan++) {
		inPtr = signal->channel[chan] + windowFrame * signalLength;
		outPtr = buffer->channel[chan] + signalDispPtr->bufferCount;
		for (j = 0; j < signalLength; j++)
			*outPtr++ = *inPtr++;
	}
	if (signalDispPtr->bufferCount < buffer->length) {
		signalDispPtr->bufferCount += signalLength;
	}

}

/**************************** SetDisplay **************************************/

/*
 * This routine sets up the canvas display control parameters.
 */

void
SetDisplay_SignalDisp(EarObjectPtr data)
{
	/* static const char *funcName = "SetDisplay_SignalDisp"; */

	/* "Manual" connection of output signal to the summary EarObject. */
	signalDispPtr->summary->inSignal[0] = data->outSignal;
	reduceChansPtr = &signalDispPtr->reduceChans;
	SetMode_Utility_ReduceChannels("average");
	SetNumChannels_Utility_ReduceChannels(1);
	Process_Utility_ReduceChannels(signalDispPtr->summary);

	signalDispPtr->redrawFlag = TRUE;
	signalDispPtr->display->canvas->InitGraph((signalDispPtr->buffer)?
	  signalDispPtr->buffer: data, signalDispPtr->summary);

}

/**************************** ShowSignal **************************************/

/*
 * This this routine creates a canvas for a signal display chart.
 * At present it MUST BE USED IN LINE.
 * After initialisation the canvas' earObject is unregistered from the main
 * list.  This means that the canvas is responsible for deleting the memory
 * when it is finished with it.
 * If this module is in-line, then the output signal is set to the input
 * input signal.
 * The call to SetProcessContinuity must be done before the setting of the
 * display parameters, otherwise the correct time is not set for the display
 * in segmented mode.
 */

BOOLN
ShowSignal_SignalDisp(EarObjectPtr data)
{
	static const char *funcName = "ShowSignal_SignalDisp";
	time_t	startTime;
	int		i;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_SignalDisp())
		return(FALSE);
	if (!SetProcessMode_SignalDisp(data)) {
		NotifyError("%s: Could not set process mode.", funcName);
		return(FALSE);
	}
	if (signalDispPtr->mode == GRAPH_MODE_OFF) {
		if (signalDispPtr->inLineProcess)
			SetProcessContinuity_EarObject(data);
		return(TRUE);
	}
	if (!CheckData_SignalDisp(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_SignalDisp(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	if (signalDispPtr->inLineProcess)
		SetProcessContinuity_EarObject(data);
	for (i = 0, startTime = time(NULL); i < data->outSignal->numWindowFrames;
	  i++) {
		if (signalDispPtr->buffer)
			ProcessBuffer_SignalDisp(data->outSignal, signalDispPtr->buffer, i);
#		if !defined(linux)
  	  	if (signalDispPtr->frameDelay > DBL_EPSILON) {
  			while (difftime(time(NULL), startTime) < signalDispPtr->frameDelay)
  				;
  			startTime = time(NULL);
  		}
#		endif
		if (!signalDispPtr->display) {
			// Create the signalDisp frame.
			signalDispPtr->display = new DisplayS(wxGetApp().GetFrame(), (long)
			  data->handle, signalDispPtr, signalDispPtr->title, wxPoint(
			  signalDispPtr->frameXPos, signalDispPtr->frameYPos), wxSize(
			  signalDispPtr->frameWidth, signalDispPtr->frameHeight));
			SetDisplay_SignalDisp(data);
			signalDispPtr->display->Show(TRUE);
		} else {
			SetDisplay_SignalDisp(data);
#			if defined (__WXMSW__)
			signalDispPtr->display->canvas->Refresh(FALSE);
			signalDispPtr->display->canvas->ForceRepaintforMSW();
#			else
			signalDispPtr->display->canvas->Refresh(FALSE);
#			endif
		}
	}
	return(TRUE);

}