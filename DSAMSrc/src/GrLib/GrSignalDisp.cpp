/******************
 *		
 * File:		StdSignalDisp.c/GrSignalDisp.cpp
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
#include "UtString.h"

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
#	include <wx/wxexpr.h>
#	include "GrUtils.h"
#	include "ExtIPCServer.h"
#	include "GrSimMgr.h"
#	include "GrLines.h"
#	include "GrSignalDisp.h"
#	include "GrCanvas.h"
#	include "GrDisplayS.h"
#	include "GrSDICanvas.h"
#	include "GrSDIFrame.h"
#else
#	include "GrSignalDisp.h"
#endif

#ifndef DEBUG
#	define	DEBUG
#endif

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

SignalDispPtr	signalDispPtr = NULL;

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

				{ "Signal",		DISPLAY_MAGNIFICATION },
				{ "Y-axis",		DISPLAY_Y_AXIS_TITLE },
				{ "X-axis",		DISPLAY_X_AXIS_TITLE },
				{ "General",	DISPLAY_WINDOW_TITLE },
				{ "", 			DISPLAY_NULL }
			};
	;
	return(&list[index]);

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

					{ "AUTO",			GRAPH_Y_AXIS_MODE_AUTO },
					{ "CHANNEL",		GRAPH_Y_AXIS_MODE_CHANNEL },
					{ "LINEAR_SCALE",	GRAPH_Y_AXIS_MODE_LINEAR_SCALE },
					{ "", 				GRAPH_Y_AXIS_MODE_NULL }
				};
	signalDispPtr->yAxisModeList = list;
	return(TRUE);

}

/****************************** InitYNormModeList ******************************/

/*
 * This routine intialises the signal line mode list array.
 */

BOOLN
InitYNormModeList_SignalDisp(void)
{
	static NameSpecifier	list[] = {

					{"MIDDLE",		GRAPH_LINE_YNORM_MIDDLE_MODE },
					{"BOTTOM",		GRAPH_LINE_YNORM_BOTTOM_MODE },
					{"",			GRAPH_LINE_YNORM_MODE_NULL }
				};
	signalDispPtr->yNormModeList = list;
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
	
	signalDispPtr->autoYScaleFlag = TRUE;
	signalDispPtr->yAxisModeFlag = TRUE;
	signalDispPtr->yNormalisationModeFlag = TRUE;
	signalDispPtr->modeFlag = TRUE;
	signalDispPtr->numGreyScalesFlag = TRUE;
	signalDispPtr->magnificationFlag = TRUE;
	signalDispPtr->summaryDisplayFlag = TRUE;
	signalDispPtr->xResolutionFlag = TRUE;
	signalDispPtr->minYFlag = FALSE;
	signalDispPtr->maxYFlag = FALSE;
	signalDispPtr->autoXScaleFlag = TRUE;
	signalDispPtr->xOffsetFlag = TRUE;
	signalDispPtr->xExtentFlag = TRUE;
	signalDispPtr->widthFlag = TRUE;
	signalDispPtr->frameDelayFlag = TRUE;
	signalDispPtr->frameHeightFlag = TRUE;
	signalDispPtr->frameWidthFlag = TRUE;
	signalDispPtr->frameXPosFlag = TRUE;
	signalDispPtr->frameYPosFlag = TRUE;
	signalDispPtr->titleFlag = FALSE;
	signalDispPtr->topMarginFlag = TRUE;
	signalDispPtr->channelStepFlag = TRUE;
	signalDispPtr->xTicksFlag = FALSE;
	signalDispPtr->xAxisTitleFlag = FALSE;
	signalDispPtr->xNumberFormatFlag = FALSE;
	signalDispPtr->yAxisTitleFlag = FALSE;
	signalDispPtr->yNumberFormatFlag = FALSE;
	signalDispPtr->xDecPlacesFlag = FALSE;
	signalDispPtr->yDecPlacesFlag = FALSE;
	signalDispPtr->yTicksFlag = FALSE;
	signalDispPtr->yInsetScaleFlag = FALSE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	signalDispPtr->parSpec = parSpec;
	signalDispPtr->autoYScale = GENERAL_BOOLEAN_ON;
	signalDispPtr->frameDelay = DISPLAY_DEFAULT_FRAME_DELAY;
	signalDispPtr->frameHeight = DISPLAY_DEFAULT_FRAME_HEIGHT;
	signalDispPtr->frameWidth = DISPLAY_DEFAULT_FRAME_WIDTH;
#	ifndef STANDARD_C_SIGNALDISP_COMPILE
	wxGetApp().GetDefaultDisplayPos(&signalDispPtr->frameXPos, &signalDispPtr->
	  frameYPos);
#	endif
	signalDispPtr->yAxisMode = GRAPH_Y_AXIS_MODE_AUTO;
	signalDispPtr->yNormalisationMode = GRAPH_LINE_YNORM_MIDDLE_MODE;
	signalDispPtr->mode = GRAPH_MODE_LINE;
	signalDispPtr->numGreyScales = GRAPH_NUM_GREY_SCALES;
	signalDispPtr->magnification = DEFAULT_SIGNAL_Y_SCALE;
	signalDispPtr->summaryDisplay = GENERAL_BOOLEAN_OFF;
	signalDispPtr->topMargin = GRAPH_TOP_MARGIN_PERCENT;
	signalDispPtr->width = DEFAULT_WIDTH;
	signalDispPtr->channelStep = DEFAULT_CHANNEL_STEP;
	signalDispPtr->xTicks = DEFAULT_X_TICKS;
	strcpy(signalDispPtr->xNumberFormat, DEFAULT_X_NUMBER_FORMAT);
	strcpy(signalDispPtr->yNumberFormat, DEFAULT_Y_NUMBER_FORMAT);
	signalDispPtr->xDecPlaces = DEFAULT_X_DEC_PLACES;
	signalDispPtr->yDecPlaces = DEFAULT_Y_DEC_PLACES;
	signalDispPtr->yTicks = DEFAULT_Y_TICKS;
	signalDispPtr->yInsetScale = GENERAL_BOOLEAN_ON;
	signalDispPtr->maxY = 0.0;
	signalDispPtr->minY = 0.0;
	signalDispPtr->title[0] = '\0';
	signalDispPtr->xAxisTitle[0] = '\0';
	signalDispPtr->yAxisTitle[0] = '\0';
	signalDispPtr->xResolution = DEFAULT_X_RESOLUTION;
	signalDispPtr->autoXScale = GENERAL_BOOLEAN_ON;
	signalDispPtr->xExtent = -1.0;
	signalDispPtr->xOffset = 0.0;
	
	InitModeList_SignalDisp();
	InitYAxisModeList_SignalDisp();
	InitYNormModeList_SignalDisp();
	if (!SetUniParList_SignalDisp()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_SignalDisp();
		return(FALSE);
	}
	signalDispPtr->bufferCount = 0;
	signalDispPtr->buffer = NULL;
	signalDispPtr->data = NULL;
	signalDispPtr->summary = NULL;
	signalDispPtr->inLineProcess = FALSE;
	signalDispPtr->redrawGraphFlag = FALSE;
	signalDispPtr->drawCompletedFlag = TRUE;
	signalDispPtr->display = NULL;
	signalDispPtr->dialog = NULL;
	signalDispPtr->critSect = NULL;

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
#	ifndef STANDARD_C_SIGNALDISP_COMPILE
	if (signalDispPtr->display)
		delete signalDispPtr->display;
#	endif
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
	SetPar_UniParMgr(&pars[DISPLAY_AUTOMATIC_SCALING], "AUTO_SCALING",
	  "Automatic scaling ('on' or 'off').",
	  UNIPAR_BOOL,
	  &signalDispPtr->autoYScale, NULL,
	  (void * (*)) SetAutoYScale_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_AUTO_Y_SCALE], "AUTO_Y_SCALE",
	  "Automatic y-axis scale ('on' or 'off').",
	  UNIPAR_BOOL,
	  &signalDispPtr->autoYScale, NULL,
	  (void * (*)) SetAutoYScale_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_CHANNEL_STEP], "CHANNEL_STEP",
	  "Channel stepping mode.",
	  UNIPAR_INT,
	  &signalDispPtr->channelStep, NULL,
	  (void * (*)) SetChannelStep_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_WINDOW_TITLE], "WIN_TITLE",
	  "Display window title.",
	  UNIPAR_STRING,
	  &signalDispPtr->title, NULL,
	  (void * (*)) SetTitle_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_FRAME_DELAY], "FRAMEDELAY",
	  "Delay between display frames (s)",
	  UNIPAR_REAL,
	  &signalDispPtr->frameDelay, NULL,
	  (void * (*)) SetFrameDelay_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_MAGNIFICATION], "MAGNIFICATION",
	  "Signal magnification.",
	  UNIPAR_REAL,
	  &signalDispPtr->magnification, NULL,
	  (void * (*)) SetMagnification_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_MAX_Y], "MAXY",
	  "Maximum Y value (for manual scaling).",
	  UNIPAR_REAL,
	  &signalDispPtr->maxY, NULL,
	  (void * (*)) SetMaxY_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_MIN_Y],"MINY",
	  "Minimum Y Value (for manual scaling).",
	  UNIPAR_REAL,
	  &signalDispPtr->minY, NULL,
	  (void * (*)) SetMinY_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_MODE], "MODE",
	  "Display mode ('off', 'line' or 'gray_scale').",
	  UNIPAR_NAME_SPEC,
	  &signalDispPtr->mode, signalDispPtr->modeList,
	  (void * (*)) SetMode_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_NUMGREYSCALES], "NUMGREYSCALES",
	  "Number of grey scales.",
	  UNIPAR_INT,
	  &signalDispPtr->numGreyScales, NULL,
	  (void * (*)) SetNumGreyScales_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_SUMMARY_DISPLAY], "SUMMARYDISPLAY",
	  "Summary display mode ('on' or 'off').",
	  UNIPAR_BOOL,
	  &signalDispPtr->summaryDisplay, NULL,
	  (void * (*)) SetSummaryDisplay_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_TOP_MARGIN], "TOPMARGIN",
	  "Top margin for display (percent of display height).",
	  UNIPAR_REAL,
	  &signalDispPtr->topMargin, NULL,
	  (void * (*)) SetTopMargin_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_WIDTH], "WIDTH",
	  "Displayed signal width (seconds or x units).",
	  UNIPAR_REAL,
	  &signalDispPtr->width, NULL,
	  (void * (*)) SetWidth_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_WINDOW_HEIGHT], "WIN_HEIGHT",
	  "Display frame height (pixel units).",
	  UNIPAR_INT,
	  &signalDispPtr->frameHeight, NULL,
	  (void * (*)) SetFrameHeight_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_WINDOW_WIDTH], "WIN_WIDTH",
	  "Display frame width (pixel units).",
	  UNIPAR_INT,
	  &signalDispPtr->frameWidth, NULL,
	  (void * (*)) SetFrameWidth_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_WINDOW_X_POS], "WIN_X_POS",
	  "Display frame X position (pixel units).",
	  UNIPAR_INT,
	  &signalDispPtr->frameXPos, NULL,
	  (void * (*)) SetFrameXPos_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_WINDOW_Y_POS], "WIN_Y_POS",
	  "Display frame Y position (pixel units).",
	  UNIPAR_INT,
	  &signalDispPtr->frameYPos, NULL,
	  (void * (*)) SetFrameYPos_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_X_RESOLUTION], "X_RESOLUTION",
	  "Resolution of X scale (1 - low, fractions are higher).",
	  UNIPAR_REAL,
	  &signalDispPtr->xResolution, NULL,
	  (void * (*)) SetXResolution_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_Y_NORMALISATION_MODE], "NORMALISATION",
	  "Y normalisation mode ('bottom' or 'middle').",
	  UNIPAR_NAME_SPEC,
	  &signalDispPtr->yNormalisationMode, signalDispPtr->yNormModeList,
	  (void * (*)) SetYNormalisationMode_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_Y_AXIS_TITLE], "Y_AXIS_TITLE",
	  "Y-axis title.",
	  UNIPAR_STRING,
	  &signalDispPtr->yAxisTitle, NULL,
	  (void * (*)) SetYAxisTitle_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_Y_AXIS_MODE], "Y_AXIS_MODE",
	  "Y-axis mode ('channel' (No.) or 'scale').",
	  UNIPAR_NAME_SPEC,
	  &signalDispPtr->yAxisMode, signalDispPtr->yAxisModeList,
	  (void * (*)) SetYAxisMode_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_Y_NUMBER_FORMAT], "Y_NUMBER_FORMAT",
	  "Y axis scale number format, (e.g. y.yye-3).",
	  UNIPAR_STRING,
	  &signalDispPtr->yNumberFormat, NULL,
	  (void * (*)) SetYNumberFormat_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_Y_DEC_PLACES], "Y_DEC_PLACES",
	  "Y axis scale decimal places.",
	  UNIPAR_INT,
	  &signalDispPtr->yDecPlaces, NULL,
	  (void * (*)) SetYDecPlaces_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_Y_TICKS], "Y_TICKS",
	  "Y axis tick marks.",
	  UNIPAR_INT,
	  &signalDispPtr->yTicks, NULL,
	  (void * (*)) SetYTicks_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_Y_INSET_SCALE], "Y_INSET_SCALE",
	  "Y inset scale mode ('on' or 'off').",
	  UNIPAR_BOOL,
	  &signalDispPtr->yInsetScale, NULL,
	  (void * (*)) SetYInsetScale_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_X_AXIS_TITLE], "X_AXIS_TITLE",
	  "X axis title.",
	  UNIPAR_STRING,
	  &signalDispPtr->xAxisTitle, NULL,
	  (void * (*)) SetXAxisTitle_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_X_NUMBER_FORMAT], "X_NUMBER_FORMAT",
	  "X axis scale number format, (e.g. x.xxe-3).",
	  UNIPAR_STRING,
	  &signalDispPtr->xNumberFormat, NULL,
	  (void * (*)) SetXNumberFormat_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_X_DEC_PLACES], "X_DEC_PLACES",
	  "X axis scale decimal places.",
	  UNIPAR_INT,
	  &signalDispPtr->xDecPlaces, NULL,
	  (void * (*)) SetXDecPlaces_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_X_TICKS], "X_TICKS",
	  "X axis tick marks.",
	  UNIPAR_INT,
	  &signalDispPtr->xTicks, NULL,
	  (void * (*)) SetXTicks_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_AUTO_X_SCALE], "AUTO_X_SCALE",
	  "Autoscale option for x-axis ('on' or 'off')",
	  UNIPAR_BOOL,
	  &signalDispPtr->autoXScale, NULL,
	  (void * (*)) SetAutoXScale_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_X_OFFSET], "X_OFFSET",
	  "X offset for display in zoom mode (x units).",
	  UNIPAR_REAL,
	  &signalDispPtr->xOffset, NULL,
	  (void * (*)) SetXOffset_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_X_EXTENT], "X_EXTENT",
	  "X extent for display in zoom mode (x units or -1 for end of signal).",
	  UNIPAR_REAL,
	  &signalDispPtr->xExtent, NULL,
	  (void * (*)) SetXExtent_SignalDisp);

	SetDefaulEnabledPars_SignalDisp();

	return(TRUE);

}

/********************************* SetDefaulEnabledPars ***********************/

/*
 * This routine sets the parameter list so that the correct default parameters
 * are enabled/disabled.
 */

BOOLN
SetDefaulEnabledPars_SignalDisp(void)
{
	static const char *funcName = "SetDefaulEnabledPars_SignalDisp";
	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	signalDispPtr->parList->pars[DISPLAY_X_DEC_PLACES].enabled = FALSE;
	signalDispPtr->parList->pars[DISPLAY_Y_DEC_PLACES].enabled = FALSE;
	signalDispPtr->parList->pars[DISPLAY_WIDTH].enabled = FALSE;
	signalDispPtr->parList->pars[DISPLAY_AUTOMATIC_SCALING].enabled = FALSE;
	
	SetAutoYScale_SignalDisp(BooleanList_NSpecLists(signalDispPtr->autoYScale)->
	  name);
	SetAutoXScale_SignalDisp(BooleanList_NSpecLists(signalDispPtr->autoXScale)->
	  name);
	/* The following should not be set from the GUI */
	signalDispPtr->parList->pars[DISPLAY_WINDOW_HEIGHT].enabled = FALSE;
	signalDispPtr->parList->pars[DISPLAY_WINDOW_WIDTH].enabled = FALSE;
	signalDispPtr->parList->pars[DISPLAY_WINDOW_X_POS].enabled = FALSE;
	signalDispPtr->parList->pars[DISPLAY_WINDOW_Y_POS].enabled = FALSE;
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
/****************************** SetAutoYScaleParsState ************************/

/*
 * This function sets enables or disables the auto Y scale relatated parameter.
 */

void
SetAutoYScaleParsState_SignalDisp(BOOLN state)
{
	signalDispPtr->parList->pars[DISPLAY_MAX_Y].enabled = state;
	signalDispPtr->parList->pars[DISPLAY_MIN_Y].enabled = state;
	if (signalDispPtr->yAxisMode == GRAPH_Y_AXIS_MODE_CHANNEL) {
		signalDispPtr->parList->pars[DISPLAY_Y_NUMBER_FORMAT].enabled = FALSE;
		signalDispPtr->parList->pars[DISPLAY_Y_TICKS].enabled = FALSE;
	} else {
		signalDispPtr->parList->pars[DISPLAY_Y_NUMBER_FORMAT].enabled = state;
		signalDispPtr->parList->pars[DISPLAY_Y_TICKS].enabled = state;
	}
	signalDispPtr->parList->updateFlag = TRUE;

}

/**************************** SetAutoYScale ***********************************/

/*
 * This routine sets the automatic scaling mode for the display.
 * If set to true, the scaling will be direct and related only to the
 * signal yScale parameter.
 */

BOOLN
SetAutoYScale_SignalDisp(char *theAutoYScale)
{
	static const char *funcName = "SetAutoYScale_SignalDisp";
	int		specifier;

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theAutoYScale,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal switch state (%s).", funcName, theAutoYScale);
		return(FALSE);
	}
	signalDispPtr->autoYScale = specifier;
	signalDispPtr->autoYScaleFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	SetAutoYScaleParsState_SignalDisp(!signalDispPtr->autoYScale);
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

/**************************** SetYAxisTitle ***********************************/

/*
 * This routine sets the 'y_axis_title' field of the 'signalDisp' structure.
 */

BOOLN
SetYAxisTitle_SignalDisp(char *yAxisTitle)
{
	static const char *funcName = "SetYAxisTitle_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	signalDispPtr->yAxisTitleFlag = TRUE;
	snprintf(signalDispPtr->yAxisTitle, MAXLINE, "%s", yAxisTitle);
	return(TRUE);

}

/**************************** SetYAxisModeParsState ***************************/

/*
 * This routine sets the module's y-axis mode related parameters state.
 */

void
SetYAxisModeParsState_SignalDisp(BOOLN state)
{
	SetAutoXScaleParsState_SignalDisp(!state);
	signalDispPtr->parList->pars[DISPLAY_Y_INSET_SCALE].enabled = state;
	signalDispPtr->parList->updateFlag = TRUE;

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
	SetYAxisModeParsState_SignalDisp(signalDispPtr->yAxisMode ==
	  GRAPH_Y_AXIS_MODE_CHANNEL);
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
	  signalDispPtr->yNormModeList)) == GRAPH_LINE_YNORM_MODE_NULL) {
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
	/* signalDispPtr->width = width; parameter to be removed. */
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
	if ((specifier == GRAPH_MODE_GREY_SCALE) && (signalDispPtr->autoYScale ==
	  GENERAL_BOOLEAN_OFF)) {
		NotifyError("%s: Automatic Y scaling must be on to use the grey-scale "
		  "mode.", funcName);
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
	signalDispPtr->titleFlag = TRUE;
	snprintf(signalDispPtr->title, MAXLINE, "%s", title);
	signalDispPtr->redrawGraphFlag = TRUE;
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

/**************************** SetXAxisTitle ***********************************/

/*
 * This routine sets the 'x_axis_title' field of the 'signalDisp' structure.
 */

BOOLN
SetXAxisTitle_SignalDisp(char *xAxisTitle)
{
	static const char *funcName = "SetXAxisTitle_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	signalDispPtr->xAxisTitleFlag = TRUE;
	snprintf(signalDispPtr->xAxisTitle, MAXLINE, "%s", xAxisTitle);
	return(TRUE);

}

/**************************** SetXNumberFormat ********************************/

/*
 * This routine sets the displayed number format, i.e. the number of decimal
 * places displayed and the exponent for the display x-axis.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetXNumberFormat_SignalDisp(char *xNumberFormat)
{
	static const char *funcName = "SetXNumberFormat_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!xNumberFormat || (xNumberFormat[0] == '\0')) {
		NotifyError("%s: This setting must be in the form 'xx.e-3' (%s)",
		  funcName, xNumberFormat);
		return(FALSE);
	}
	snprintf(signalDispPtr->xNumberFormat, MAXLINE, "%s", xNumberFormat);
	signalDispPtr->xNumberFormatFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/****************************** SetAutoXScaleParsState ************************/

/*
 * This function sets enables or disables the auto scale relatated parameter.
 */

void
SetAutoXScaleParsState_SignalDisp(BOOLN state)
{
	signalDispPtr->parList->pars[DISPLAY_X_NUMBER_FORMAT].enabled = state;
	signalDispPtr->parList->pars[DISPLAY_X_TICKS].enabled = state;
	signalDispPtr->parList->pars[DISPLAY_X_OFFSET].enabled = state;
	signalDispPtr->parList->pars[DISPLAY_X_EXTENT].enabled = state;
	signalDispPtr->parList->updateFlag = TRUE;

}

/****************************** SetAutoXScale *********************************/

/*
 * This function sets the module's autoXScale parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAutoXScale_SignalDisp(char *theAutoXScale)
{
	static const char	*funcName = "SetAutoXScale_SignalDisp";
	int		specifier;

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theAutoXScale,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal switch state (%s).", funcName,
		  theAutoXScale);
		return(FALSE);
	}
	signalDispPtr->autoXScale = specifier;
	signalDispPtr->autoXScaleFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	SetAutoXScaleParsState_SignalDisp(!signalDispPtr->autoXScale);
	return(TRUE);

}

/****************************** SetXOffset ************************************/

/*
 * This function sets the module's xOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetXOffset_SignalDisp(double theXOffset)
{
	static const char	*funcName = "SetXOffset_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theXOffset < 0.0) {
		NotifyError("%s: Cannot set the X offset less than zero (%g units).",
		  funcName, theXOffset);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	signalDispPtr->xOffsetFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	signalDispPtr->xOffset = theXOffset;
	return(TRUE);

}

/****************************** SetXExtent ************************************/

/*
 * This function sets the module's xExtent parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetXExtent_SignalDisp(double theXExtent)
{
	static const char	*funcName = "SetXExtent_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	signalDispPtr->xExtentFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	signalDispPtr->xExtent = theXExtent;
	return(TRUE);

}

/**************************** SetYNumberFormat ********************************/

/*
 * This routine sets the decimal places displayed for the display x-axis.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetYNumberFormat_SignalDisp(char *yNumberFormat)
{
	static const char *funcName = "SetYNumberFormat_SignalDisp";

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!yNumberFormat || (yNumberFormat[0] == '\0')) {
		NotifyError("%s: This setting must be in the form 'xx.e-3' (%s)",
		  funcName, yNumberFormat);
		return(FALSE);
	}
	snprintf(signalDispPtr->yNumberFormat, MAXLINE, "%s", yNumberFormat);
	signalDispPtr->yNumberFormatFlag = TRUE;
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

/**************************** SetYInsetScale **********************************/

/*
 * This routine sets the y inste scale mode for the display.
 * If set to true, the scaling will be direct and related only to the
 * signal yScale parameter.
 */

BOOLN
SetYInsetScale_SignalDisp(char *theYInsetScale)
{
	static const char *funcName = "SetYInsetScale_SignalDisp";
	int		specifier;

	if (signalDispPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theYInsetScale,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal switch state (%s).", funcName,
		  theYInsetScale);
		return(FALSE);
	}
	signalDispPtr->yInsetScaleFlag = TRUE;
	signalDispPtr->yInsetScale = specifier;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetXDecPlaces ***********************************/

/*
 * This routine sets the decimal places displayed for the display x-axis.
 * This is a bool response, hence only non-zero values are TRUE.
 * This parameter has been disabled in the parlist.  It is obsolete.
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
 * This parameter has been disabled in the parlist.  It is obsolete.
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
	if (signalDispPtr->autoYScaleFlag && !signalDispPtr->autoYScale &&
	  (!signalDispPtr->minYFlag || !signalDispPtr->maxYFlag)) {
		NotifyError("%s: When the automatic y scale is off, the minimum and "
		  "maximum Y value must also be set.", funcName);
		return(FALSE);
	}
	if (!signalDispPtr->autoYScale && (signalDispPtr->minY >= signalDispPtr->
	  maxY)) {
		NotifyError("%s: the minimum y value (%g) should be less than than the "
		  "maximum y value (%g).", funcName, signalDispPtr->minY,
		  signalDispPtr->maxY);
		return(FALSE);
	}
	if (signalDispPtr->autoXScaleFlag && !signalDispPtr->autoXScale &&
	  (!signalDispPtr->xOffsetFlag || !signalDispPtr->xExtentFlag)) {
		NotifyError("%s: When automatic x-scale is off, the x offset and "
		  "x extent values must both be set.", funcName);
		return(FALSE);
	}
	return(TRUE);

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
	  BooleanList_NSpecLists(signalDispPtr->autoYScale)->name);
	DPrint("\tChannel step for display: %d\n", signalDispPtr->channelStep);
	if (!signalDispPtr->autoYScale)
		DPrint("\tMinimum/Maximum Y values: %g/%g units.\n",
		  signalDispPtr->minY, signalDispPtr->maxY);
	DPrint("\tY normalisation mode: %s\n",
	  signalDispPtr->yNormModeList[signalDispPtr->yNormalisationMode].name);
	DPrint("\tSummary display mode: %s\n",
	  BooleanList_NSpecLists(signalDispPtr->summaryDisplay)->name);
	DPrint("\tSignal Y scale = %g.\n", signalDispPtr->magnification);
	DPrint("\tY axis title = %s\n", signalDispPtr->yAxisTitle);
	DPrint("\tY-axis mode = %s,\n",
	  signalDispPtr->yAxisModeList[signalDispPtr->yAxisMode].name);
	DPrint("\tY axis scale number format = %s\n", signalDispPtr->yNumberFormat);
	DPrint("\tY axis ticks = %d\n", signalDispPtr->yTicks);
	DPrint("\tY inste scale mode: %s\n",
	  BooleanList_NSpecLists(signalDispPtr->yInsetScale)->name);
	DPrint("\tX resolution = %g (<= 1.0)\n", signalDispPtr->xResolution);
	DPrint("\tX axis title = %s\n", signalDispPtr->xAxisTitle);
	DPrint("\tX axis ticks = %d\n", signalDispPtr->xTicks);
	DPrint("\tX axis scale number format = %s\n", signalDispPtr->xNumberFormat);
	if (!signalDispPtr->autoXScale) {
		DPrint("\tOffset/extent X values: %g", signalDispPtr->xOffset);
		if (signalDispPtr->xExtent < DBL_EPSILON)
			DPrint("units / <end of signal>).\n");
		else
			DPrint("/%g units.\n", signalDispPtr->xExtent);
	}
	DPrint("\tTop margin percentage = %g %%\n", signalDispPtr->topMargin);
	DPrint("\tFrame: dimensions %d x %d\n", signalDispPtr->frameWidth,
	  signalDispPtr->frameHeight);
	DPrint("\tFrame: position %d x %d\n", signalDispPtr->frameXPos,
	  signalDispPtr->frameYPos);
	DPrint("\tDelay %g ms\n", MILLI(signalDispPtr->frameDelay));
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
	/*static const char	*funcName = "CheckData_SignalDisp"; */

	/*** Put additional checks here. ***/
	return(TRUE);

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
	double	definedDuration;
	SignalDataPtr	signal, buffer;
	
	if (signalDispPtr->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		signal = data->outSignal;
		if (signalDispPtr->autoXScale)
			definedDuration = -1.0;
		else
			definedDuration = (signalDispPtr->xExtent > DBL_EPSILON)?
			  signalDispPtr->xOffset + signalDispPtr->xExtent:
			  _GetDuration_SignalData(signal) - signalDispPtr->xOffset;
		if (signalDispPtr->updateProcessVariablesFlag || data->
		  updateProcessFlag) {
			FreeProcessVariables_SignalDisp();
#			ifndef STANDARD_C_SIGNALDISP_COMPILE
			signalDispPtr->critSect = new wxCriticalSection();
#			endif
			if ((signalDispPtr->summary = Init_EarObject(
			  "Util_ReduceChannels")) == NULL) {
				NotifyError("%s: Out of memory for summary EarObject.",
				  funcName);
				return(FALSE);
			}
			ConnectOutSignalToIn_EarObject(data, signalDispPtr->summary);
			if ((definedDuration > _GetDuration_SignalData(signal)) ||
			  (signal->numWindowFrames !=
			  SIGNALDATA_DEFAULT_NUM_WINDOW_FRAMES)) {
				if ((signalDispPtr->buffer = Init_EarObject("NULL")) == NULL) {
					NotifyError("%s: Out of memory for buffer EarObject.",
					  funcName);
					return(FALSE);
				}
				if (signal->numWindowFrames !=
				  SIGNALDATA_DEFAULT_NUM_WINDOW_FRAMES) {
					signalDispPtr->xExtent = _GetDuration_SignalData(signal) /
				      signal->numWindowFrames;
					signalDispPtr->xOffset = 0.0;
				}
			}
			if (signalDispPtr->xAxisTitle[0] == '\0')
				SetXAxisTitle_SignalDisp(data->outSignal->info.sampleTitle);
			if (signalDispPtr->yAxisTitle[0] == '\0')
				SetYAxisTitle_SignalDisp(data->outSignal->info.channelTitle);
			if (signalDispPtr->yAxisMode == GRAPH_Y_AXIS_MODE_AUTO)
				signalDispPtr->yAxisMode = (data->outSignal->numChannels > 1)?
				  GRAPH_Y_AXIS_MODE_CHANNEL: GRAPH_Y_AXIS_MODE_LINEAR_SCALE;
			signalDispPtr->parList->updateFlag = TRUE;
			signalDispPtr->updateProcessVariablesFlag = FALSE;
			data->updateProcessFlag = FALSE;
		}
		signalDispPtr->bufferCount = 0;
		if (signalDispPtr->buffer) {
			if (!InitOutSignal_EarObject(signalDispPtr->buffer, signal->
			  numChannels, (ChanLen) floor(definedDuration / signal->dt + 0.5),
			  signal->dt)) {
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

#	ifndef STANDARD_C_SIGNALDISP_COMPILE
	if (signalDispPtr->critSect)
		delete signalDispPtr->critSect;
#	endif

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

	SetPar_ModuleMgr(signalDispPtr->summary, "mode", "average");
	SetPar_ModuleMgr(signalDispPtr->summary, "num_Channels", "1");
	RunProcess_ModuleMgr(signalDispPtr->summary);
	signalDispPtr->data = data;

	signalDispPtr->redrawGraphFlag = TRUE;

}

/****************************** PostDisplayEvent ******************************/

/*
 * This routine is used by a thread to post a display event in a thread-safe
 * way.
 */

void
PostDisplayEvent_SignalDisp(void)
{
#	ifndef STANDARD_C_SIGNALDISP_COMPILE
	wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED,
	  SDIFRAME_SIM_THREAD_DISPLAY_EVENT);
	event.SetInt(MYAPP_THREAD_DRAW_GRAPH);
	event.SetClientData(signalDispPtr);
	wxPostEvent(wxGetApp().GetFrame(), event);
#	endif

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

#	ifndef STANDARD_C_SIGNALDISP_COMPILE
	static const char *funcName = "ShowSignal_SignalDisp";
	time_t	startTime;
	BOOLN	notReady;
	int		i;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_SignalDisp())
		return(FALSE);
	if (!CheckInSignal_EarObject(data, funcName))
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
		notReady = TRUE;
		while (notReady) {
			signalDispPtr->critSect->Enter();
			notReady = !signalDispPtr->drawCompletedFlag;
			signalDispPtr->critSect->Leave();
		}
		signalDispPtr->drawCompletedFlag = FALSE;
		signalDispPtr->critSect->Enter();
		SetDisplay_SignalDisp((signalDispPtr->buffer)? signalDispPtr->buffer:
		  data);
		signalDispPtr->critSect->Leave();
		PostDisplayEvent_SignalDisp();
	}
#	else
	BOOLN	RunModel_ModuleMgr_Null(EarObjectPtr data);

	return(RunModel_ModuleMgr_Null(data));

#	endif /* ! STANDARD_C_SIGNALDISP_COMPILE */
	return(TRUE);

}
