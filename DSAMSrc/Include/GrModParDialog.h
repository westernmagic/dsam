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
#include "GeUniParMgr.h"
#include "UtBandwidth.h"
#include "UtCFList.h"
#include "UtIonChanList.h"

#include "GrDSAMList.h"
#include "GrParControl.h"
#include "GrDialogInfo.h"
#include "GrParListInfo.h"
#include "GrParListInfoList.h"
#include "GrDialogList.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	MODPARDIALOG_MAX_PARLISTS		6

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

enum {

	MODPARDIALOG_ID_ADD_IC = 1,
	MODPARDIALOG_ID_DELETE_IC

};

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** ModuleParDialog **********************************/

class ModuleParDialog: public DialogList {

	wxButton	*deleteICBtn, *addICBtn;

 public:
	ModuleParDialog(wxWindow *parent, const wxString& title, int theInfoNum,
	  DatumPtr pc, UniParListPtr theParList = NULL, long theStyle =
	  DIALOGLIST_STYLE_DEFAULT, int x = -1, int y = -1, int width = -1,
	  int height = -1, long style = wxDEFAULT_DIALOG_STYLE);
	~ModuleParDialog(void);

	void	DeleteDialog(void);
	void	OnICButton(wxCommandEvent& event);
	void	OnOk(wxCommandEvent& event);
	void	OnPageChanged(wxNotebookEvent &event);

	DECLARE_EVENT_TABLE()

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

