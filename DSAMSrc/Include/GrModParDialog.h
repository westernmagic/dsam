/**********************
 *
 * File:		GrModParDialog.h
 * Purpose:		Module parameter handling routines.
 *				This is a dialog for changing the parameters of a module using
 *				the module's parameter list.
 * Comments:	04-10-98 LPO: This module has been set up so that before any
 *				attempt to run the simulation, the parameters in the dialogs
 *				are always checked to see if they have been changed (without
 *				pressing <return>).
 * 				23-10-98 LPO: This Module has been taken from the GrSimMgr
 *				module which used to contain both this and the SimModuleDialog
 *				29-01-99 LPO: The 	ResetGUIDialogs() is used by the
 *				'TextProc', 'ButtonProc' and 'ListProc' routines, to ensure
 *				that error messages are given as dialogs.
 * Author:		L. P. O'Mard
 * Created:		01 Oct 1998
 * Updated:		29 Jan 1999
 * Copyright:	(c) 1999, University of Essex
 *
 **********************/
 
#ifndef _GRMODPARDIALOG_H
#define _GRMODPARDIALOG_H 1

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "UtBandwidth.h"
#include "UtCFList.h"
#include "UtIonChanList.h"

#include "GrItems.h"
#include "GrDSAMList.h"
#include "GrDialogInfo.h"
#include "GrParListInfo.h"
#include "GrParListInfoList.h"
#include "GrDialogList.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	MODULEPAR_DIALOG_MAX_PARLISTS		6

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** ModuleParDialog **********************************/

class ModuleParDialog: public DialogList {

 public:
	ModuleParDialog(wxWindow *parent, char *title, int theInfoNum, DatumPtr pc,
	  UniParListPtr theParList = NULL, BOOLN labelsFlag = TRUE, Bool modal =
	  DIALOGLIST_MODAL_MODE, int x = 300, int y = 300, int width = 500,
	  int height = 500, long style = wxDEFAULT_DIALOG_STYLE);
	~ModuleParDialog(void);

	void	ButtonProc(wxButton& but, wxCommandEvent& event);
	void	DeleteDialog(void);
	void	ListBoxProc(wxListBox& list, wxCommandEvent& WXUNUSED(event));
	void	SliderProc(wxSlider& slider, wxCommandEvent& WXUNUSED(event));
	void	TextProc(wxText& text, wxCommandEvent& event);

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

#endif

