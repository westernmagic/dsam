/******************
 *
 * File:		GeUniParMgr.h
 * Purpose:		This file contains the universal parameter manager routines
 *				I have got to check this.
 *				08-10-98 LPO: Fixed bug with name specifiers, by introducing
 *				the 'UNIPAR_NAME_SPEC' type: 'LINEAR' mode was being selected
 *				by the system when 'SINE' was asked for - I hadn't noticed that.
 *				12-10-98 LPO: Parameter lists that return structures, such as
 *				the CFList, will only carry out the setting function when all
 *				parameters have been set.
 *				06-11-98 LPO: Implemented the array handling for the likes of
 *				the StMPTone module.
 *				10-12-98 LPO: Introduced handling of NULL parLists, i.e. for 
 *				modules with no parameters.
 *				11-12-98 LPO: Introduced the second array index for the IC list.
 *				19-01-99 LPO: The 'arrayIndex[0]' always refers to the last
 *				(lowest?) index level, i.e. for the 'ICList' mode
 *				'arrayIndex[1]' refers to the ion ionChannel, and
 *				'arrayIndex[0]' refers to the respective ion channel parameter
 *				array element.
 *				27-01-99 LPO: Corrected problem in 'FindUniPar_' where it was
 *				returning before checking the rest of a parameter list after a
 *				sub module's parameter list.
 *				29-04-99 LPO: The 'FindUniPar_' routine can now find the 
 *				'CFLIST' abbreviation so that the 'CFListPtr' pointer can be
 *				accessed.
 *				19-05-99 LPO: I have changed the 'UNIPAR_FILE_NAME' code so that
 *				I can save the 'defaultExtension' to be used with the GUI.
 *				30-05-99 LPO: The strings and file names are now printed
 *				surrounded by speech marks, '"'.
 *				02-06-99 LPO: I have updated the 'FindUniPar_' routine so that
 *				it now will find the parameters within a simulation script.
 *				03-06-99 LPO: Corrected the problem with 'SetParValue_' not
 *				treating the general list parameters correctly.
 *				08-09-99 LPO: In the 'FindUniPar_' routine only the sub-
 *				parameters for a module would be set, and not the module
 *				parameter file name itself.  This has been fixed.
 *				26-10-99 LPO: Introduced the 'enabled' flag for the 'UniPar'
 *				structure.  Printing, setting operations and such like will not
 *				be carried out on 'disabled' parameters.
 * Authors:		L. P. O'Mard
 * Created:		24 Sep 1998
 * Updated:		03 Jun 1999
 * Copyright:	(c) 1999, University of Essex.
 * 
 ******************/

#ifndef	_GEUNIPARMGR_H
#define _GEUNIPARMGR_H	1

#include "GeSignalData.h"
#include "GeEarObject.h"
#include "UtDatum.h"
#include "UtBandwidth.h"
#include "UtCFList.h"
#include "UtDynaList.h"
#include "UtIonChanList.h"
#include "UtNameSpecs.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define UNIPAR_NAME_SEPARATOR		'.'
#define UNIPAR_INDEX_SEPARATOR		':'
#define UNIPAR_SUB_PAR_LIST_MARKER	"  "
#define UNIPAR_MAX_ARRAY_INDEX		2

/******************************************************************************/
/*************************** Macro definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	UNIPAR_GENERAL_NULL,
	UNIPAR_GENERAL_ARRAY_INDEX_1,
	UNIPAR_GENERAL_ARRAY_INDEX_2

} UniParGeneralParSpecifier;

typedef enum {

	UNIPAR_UNKNOWN,
	UNIPAR_BOOL,
	UNIPAR_INT,
	UNIPAR_INT_ARRAY,
	UNIPAR_LONG,
	UNIPAR_REAL,
	UNIPAR_REAL_ARRAY,
	UNIPAR_STRING,
	UNIPAR_NAME_SPEC,
	UNIPAR_NAME_SPEC_WITH_FILE,
	UNIPAR_NAME_SPEC_WITH_FPATH,
	UNIPAR_MODULE,
	UNIPAR_CFLIST,
	UNIPAR_ICLIST,
	UNIPAR_SIMSCRIPT,
	UNIPAR_FILE_NAME

} UniParTypeSpecifier;

typedef enum {

	UNIPAR_SET_GENERAL,
	UNIPAR_SET_CFLIST,
	UNIPAR_SET_IC,
	UNIPAR_SET_ICLIST,
	UNIPAR_SET_SIMSPEC

} UniParModeSpecifier;

typedef struct UniPar {

	BOOLN	enabled;
	uInt	index;
	char	*abbr;
	char	*desc;

	UniParModeSpecifier	mode;
	UniParTypeSpecifier	type;
	union {
	
		int	*		i;
		long *		l;
		double *	r;
		char *		s;
		CFListPtr	*cFPtr;
		IonChanListPtr	*iCPtr;
		struct {
			int	*			specifier;
			NameSpecifier	*list;
		} nameList;
		struct {
			char	*name;
			char	*defaultExtension;
		} file;
		struct {
			char 	*parFile;
			UniParListPtr	parList;
		} module;
		struct {
			char 	*fileName;
			DatumPtr	*simulation;
		} simScript;
		struct {
			int	*		numElements;
			int			index;
			union {
				int	**		i;
				long **		l;
				double **	r;
			} pPtr;
		} array;
	} valuePtr;
	union {

		BOOLN	(* SetInt)(int);
		BOOLN	(* SetIntArrayElement)(int, int);
		BOOLN	(* SetLong)(long);
		BOOLN	(* SetReal)(double);
		BOOLN	(* SetRealArrayElement)(int, double);
		BOOLN	(* SetString)(char *);
		BOOLN	(* SetCFList)(CFListPtr);
		BOOLN	(* SetICList)(IonChanListPtr);
		BOOLN	(* SetDatumPtr)(DatumPtr);

		BOOLN	(* SetCFListInt)(CFListPtr, int);
		BOOLN	(* SetCFListReal)(CFListPtr, double);
		BOOLN	(* SetCFListRealArrayElement)(CFListPtr, int, double);
		BOOLN	(* SetCFListString)(CFListPtr, char *);

		BOOLN	(* SetICListInt)(IonChanListPtr, int);
		BOOLN	(* SetICListReal)(IonChanListPtr, double);
		BOOLN	(* SetICListString)(IonChanListPtr, char *);

		BOOLN	(* SetICInt)(IonChannelPtr, int);
		BOOLN	(* SetICReal)(IonChannelPtr, double);
		BOOLN	(* SetICRealArrayElement)(IonChannelPtr, int, double);
		BOOLN	(* SetICString)(IonChannelPtr, char *);

	} FuncPtr;

} UniPar, *UniParPtr;

typedef struct UniParListNode {

	UniParModeSpecifier	mode;

	BOOLN	updateFlag;
	int		arrayIndex[UNIPAR_MAX_ARRAY_INDEX];
	uInt	numPars;
	UniPar	*pars;
	union {

		CFListPtr		cFs;
		IonChanListPtr	iCs;

	} handlePtr;

} UniParList;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Global Subroutines *******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckParList_UniParMgr(UniParListPtr list);

UniParPtr	FindUniPar_UniParMgr(UniParListPtr *parList, char *parName);

void	FreeList_UniParMgr(UniParListPtr *list);

NameSpecifier *	GeneralParNameList_UniParMgr(int index);

char *	GetParString_UniParMgr(UniParPtr p);

UniParListPtr	InitList_UniParMgr(UniParModeSpecifier mode, int numPars,
				  void *handlePtr);

BOOLN	ParseArrayValue_UniParMgr(UniParPtr par, char *parValue,
		  char **parValuePtr, int *index);

BOOLN	PrintArray_UniParMgr(UniParPtr p, char *suffix);

BOOLN	PrintPars_UniParMgr(UniParListPtr list, char *prefix, char *suffix);

BOOLN	PrintValue_UniParMgr(UniParPtr p);

BOOLN	ResetCFList_UniParMgr(UniParListPtr parList);

BOOLN	SetGeneralListPar_UniParMgr(UniParListPtr parList, int specifier,
		  char *parValue);

BOOLN	SetCFListParValue_UniParMgr(UniParListPtr *parList, uInt index,
		  char *parValue);

BOOLN	SetGeneralParValue_UniParMgr(UniParListPtr parList, uInt index,
		  char *parValue);

BOOLN	SetICEnabled_IonChanList(IonChannelPtr theIC, char *theICEnabled);

BOOLN	SetICListParValue_UniParMgr(UniParListPtr *parList, uInt index,
		  char *parValue);

BOOLN	SetICParValue_UniParMgr(UniParListPtr parList, uInt index,
		  char *parValue);

void	SetPar_UniParMgr(UniParPtr par, char *abbreviation, char *description,
		  UniParTypeSpecifier type, void *ptr1, void *ptr2, void * (* Func));

BOOLN	SetParValue_UniParMgr(UniParListPtr *parList, uInt index,
		  char *parValue);

__END_DECLS

#endif
