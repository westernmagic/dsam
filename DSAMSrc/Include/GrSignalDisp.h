/******************
 *
 * File:		GrSignalDisp.h
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
 *				26-11-97 LPO: Using new GeNSpecLists module for BOOLN
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
 * Author:		L.P.O'Mard
 * Created:		06 Jan 1995
 * Updated:		08 Oct 1998
 * Copyright:	(c) 1998, University of Essex
 *
 ******************/

#ifndef	_GRSIGNALDISP_H
#define _GRSIGNALDISP_H	1

#include "UtRedceChans.h"
#include "UtNameSpecs.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define DISPLAY_NUM_PARS			28

#define	DEFAULT_SIGNAL_Y_SCALE			1.0
#define	DEFAULT_X_RESOLUTION			0.01
#define	DEFAULT_WIDTH					-1.0
#define	DEFAULT_CHANNEL_STEP			1
#define	DEFAULT_X_NUMBER_FORMAT			"xe-3"
#define	DEFAULT_Y_NUMBER_FORMAT			"x"
#define	DEFAULT_X_TICKS					6
#define	DEFAULT_Y_TICKS					15
#define DEFAULT_X_DEC_PLACES			0
#define DEFAULT_Y_DEC_PLACES			0


#define	DISPLAY_PROCESS_NAME				"DSAM Display"
#define	DISPLAY_DEFAULT_FRAME_WIDTH			440
#define	DISPLAY_DEFAULT_FRAME_HEIGHT		500
#define	DISPLAY_DEFAULT_FRAME_DELAY			0.0

/******************************************************************************/
/*************************** Pre-reference definitions ************************/
/******************************************************************************/

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)

	class DisplayS;
	class DialogList;
	class wxCriticalSection;
	
#endif /* GRAPHICS_SUPPORT */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef	enum {

	/* Signal Controls */
	DISPLAY_AUTOMATIC_SCALING,
	DISPLAY_CHANNEL_STEP,
	DISPLAY_MAGNIFICATION,
	DISPLAY_MAX_Y,
	DISPLAY_MIN_Y,
	DISPLAY_NUMGREYSCALES,
	DISPLAY_WIDTH,
	DISPLAY_X_RESOLUTION,
	DISPLAY_Y_NORMALISATION_MODE,
	/* Axis controls */
	DISPLAY_X_AXIS_TITLE,
	DISPLAY_X_NUMBER_FORMAT,
	DISPLAY_X_DEC_PLACES,
	DISPLAY_X_TICKS,
	DISPLAY_Y_AXIS_TITLE,
	DISPLAY_Y_AXIS_MODE,
	DISPLAY_Y_NUMBER_FORMAT,
	DISPLAY_Y_DEC_PLACES,
	DISPLAY_Y_TICKS,
	DISPLAY_Y_INSET_SCALE,
	/* General Controls */
	DISPLAY_FRAME_DELAY,
	DISPLAY_MODE,
	DISPLAY_SUMMARY_DISPLAY,
	DISPLAY_TOP_MARGIN,
	DISPLAY_WINDOW_HEIGHT,
	DISPLAY_WINDOW_TITLE,
	DISPLAY_WINDOW_WIDTH,
	DISPLAY_WINDOW_X_POS,
	DISPLAY_WINDOW_Y_POS,
	DISPLAY_NULL

} SignalDispParSpecifier;

typedef	enum {

	GRAPH_MODE_OFF,
	GRAPH_MODE_LINE,
	GRAPH_MODE_GREY_SCALE,
	GRAPH_MODE_NULL

} SignalDispModeSpecifier;

typedef	enum {

	GRAPH_Y_AXIS_MODE_CHANNEL,
	GRAPH_Y_AXIS_MODE_LINEAR_SCALE,
	GRAPH_Y_AXIS_MODE_NULL

} SignalDispyAxisModeSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	automaticYScalingFlag, channelStepFlag, frameHeightFlag;
	BOOLN	frameWidthFlag, frameXPosFlag, frameYPosFlag, yAxisModeFlag;
	BOOLN	yNormalisationModeFlag, modeFlag, numGreyScalesFlag;
	BOOLN	summaryDisplayFlag, xTicksFlag, xNumberFormatFlag;
	BOOLN	yNumberFormatFlag, xDecPlacesFlag, yDecPlacesFlag;
	BOOLN	yTicksFlag, yInsetScaleFlag, frameDelayFlag, titleFlag;
	BOOLN	magnificationFlag, xResolutionFlag, minYFlag, maxYFlag;
	BOOLN	topMarginFlag, widthFlag, xAxisTitleFlag, yAxisTitleFlag;

	BOOLN	updateProcessVariablesFlag;
	char	title[MAXLINE];
	char	xAxisTitle[MAXLINE];
	char	yAxisTitle[MAXLINE];
	char	xNumberFormat[MAXLINE];
	char	yNumberFormat[MAXLINE];
	int		automaticYScaling;
	int		channelStep;
	int		frameHeight;
	int		frameWidth;
	int		frameXPos;
	int		frameYPos;
	int		yAxisMode;
	int		xDecPlaces;
	int		yDecPlaces;
	int		yNormalisationMode;
	int		mode;
	int		numGreyScales;
	int		summaryDisplay;
	int		xTicks;
	int		yTicks;
	int		yInsetScale;
	double	frameDelay;
	double	magnification;
	double	xResolution;
	double	minY;
	double	maxY;
	double	topMargin;
	double	width;
	
	/* Private variables */
	NameSpecifier	*parNameList;
	NameSpecifier	*modeList;
	NameSpecifier	*yAxisModeList;
	UniParListPtr	parList;
	BOOLN			inLineProcess;
	BOOLN			redrawGraphFlag;
	BOOLN			drawCompletedFlag;
	ChanLen			bufferCount;
	EarObjectPtr	buffer;
	EarObjectPtr	data;
	EarObjectPtr	summary;
#	if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
	DisplayS		*display;
	DialogList		*dialog;
	wxCriticalSection	*critSect;
#	endif /* GRAPHICS_SUPPORT */

} SignalDisp, *SignalDispPtr;

/******************************************************************************/
/*************************** External variables *******************************/
/******************************************************************************/

extern SignalDisp	signalDisp, *signalDispPtr;

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_SignalDisp(EarObjectPtr data);

void	CheckForDisplay_SignalDisp(long handle);

BOOLN	CheckPars_SignalDisp(void);

BOOLN	Free_SignalDisp(void);

BOOLN	FreeProcessVariables_SignalDisp(void);

NameSpecifier *	GetPanelList_SignalDisp(int index);

UniParListPtr	GetUniParListPtr_SignalDisp(void);

BOOLN	Init_SignalDisp(ParameterSpecifier parSpec);

BOOLN	InitBOOLNeanList_SignalDisp(void);

BOOLN	InitModeList_SignalDisp(void);

BOOLN	InitParNameList_SignalDisp(void);

BOOLN	InitProcessVariables_SignalDisp(EarObjectPtr data);

BOOLN	InitYAxisModeList_SignalDisp(void);

void	PostDisplayEvent_SignalDisp(void);

BOOLN	PrintPars_SignalDisp(void);

void	ProcessBuffer_SignalDisp(SignalDataPtr signal,
		  EarObjectPtr bufferEarObj, int windowFrame);

BOOLN	SetAutomaticYScaling_SignalDisp(char *theAutomaticYScaling);

BOOLN	SetChannelStep_SignalDisp(int theChannelStep);

BOOLN	SetFrameDelay_SignalDisp(double theFrameDelay);

BOOLN	SetFrameHeight_SignalDisp(int theFrameHeight);

BOOLN	SetFrameXPos_SignalDisp(int theFrameXPos);

BOOLN	SetFrameYPos_SignalDisp(int theFrameYPos);

BOOLN	SetFrameWidth_SignalDisp(int theFrameWidth);

BOOLN	SetMaxY_SignalDisp(double maxY);

BOOLN	SetMinY_SignalDisp(double minY);

BOOLN	SetMode_SignalDisp(char *theMode);

BOOLN	InitModule_SignalDisp(ModulePtr theModule);

BOOLN	SetNumGreyScales_SignalDisp(int theNumGreyScales);

BOOLN	SetParsPointer_SignalDisp(ModulePtr theModule);

BOOLN	SetProcessMode_SignalDisp(EarObjectPtr data);

BOOLN	SetMagnification_SignalDisp(double magnification);

BOOLN	SetSummaryDisplay_SignalDisp(char *summaryDisplay_SignalDis);

BOOLN	SetTitle_SignalDisp(char *title);

BOOLN	SetTopMargin_SignalDisp(double topMargin);

BOOLN	SetUniParList_SignalDisp(void);

BOOLN	SetWidth_SignalDisp(double width);

BOOLN	SetXAxisTitle_SignalDisp(char *xAxisTitle);

BOOLN	SetXDecPlaces_SignalDisp(int xDecPlaces);

BOOLN	SetXResolution_SignalDisp(double xResolution);

BOOLN	SetXNumberFormat_SignalDisp(char *xNumberFormat);

BOOLN	SetXTicks_SignalDisp(int xTicks);

BOOLN	SetYAxisMode_SignalDisp(char *theYAxisMode);

BOOLN	SetYAxisTitle_SignalDisp(char *yAxisTitle);

BOOLN	SetYDecPlaces_SignalDisp(int yDecPlaces);

BOOLN	SetYNumberFormat_SignalDisp(char *yNumberFormat);

BOOLN	SetYInsetScale_SignalDisp(char *yInsetScale);

BOOLN	SetYNormalisationMode_SignalDisp(char *theYNormalisationMode);

BOOLN	SetYTicks_SignalDisp(int yTicks);

BOOLN	ShowSignal_SignalDisp(EarObjectPtr data);

__END_DECLS

#endif
