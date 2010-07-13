/*
 * libRunDSAMSim.cpp
 *
 *  Created on: 20 Jun 2010
 *      Author: lowel
 *  ToDo:	Return signal information as class fields.
 */

#if wxUSE_GUI
    #error "This sample can't be compiled in GUI mode."
#endif // wxUSE_GUI

#include <jni.h>
#include <stdio.h>
#include <iostream>

#include <wx/socket.h>
#include <wx/app.h>

#include <GeCommon.h>
#include <GeSignalData.h>
#include <GeEarObject.h>
#include <GeUniParMgr.h>
#include <UtString.h>
#include <ExtIPCUtils.h>
#include <ExtMainApp.h>
#include <ExtIPCClient.h>
#include <UtDatum.h>
#include <UtAppInterface.h>

#include "MatInSignal.h"
#include "MatMainApp.h"
#include "libRunDSAMSim.h"

/******************************************************************************/
/*************************** Global Namspaces *********************************/
/******************************************************************************/

using namespace std;

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	USAGE_MESSAGE \
  "RunDSAMSim:\n" \
  "\n" \
  "[data info] = RunDSAMSim(<sim file>, [<parameter settings>, [<signal>," \
  "...\n" \
  "                         <signal info.>]])\n" \
  "\n" \
  "<sim file>           Simulation file name (string).\n" \
  "<parameter settings> Parameter '<name> <value> ...' pairs (string).\n" \
  "<signal>             Data signal ([chan, samples] real matrix).\n" \
  "<signal info.>       Signal information (structure).\n"

/******************************************************************************/
/*************************** Class Definitions ********************************/
/******************************************************************************/

class LibRunDSAMSim {
	bool	staticTimeFlag;
	short	numChannels, interleaveLevel, numWindowFrames;
	ChanLen	length;
	Float	**inChannels, dt, outputTimeOffset;
	EarObjectPtr	audModel;
	SignalDataPtr	outSignal;
	MatInSignal *inputSignal;
	MatMainApp	*mainApp;
	wxString	simFile;
	wxString	parOptions;
	JNIEnv 		*env;
	jclass	 	cls;
	jobjectArray	outChannels;

  public:
	LibRunDSAMSim(JNIEnv *env, jclass cls);
	~LibRunDSAMSim(void);

	bool	GetBooleanField(const wxString &name);
	Float	GetFloatField(const wxString &name);
	int		GetIntField(const wxString &name);
	bool	GetInfoFields(void);
	long	GetLongField(const wxString &name);
	short	GetShortField(const wxString &name);
	bool	InitInputSignal(jobjectArray inChannels);
	bool	InitOutChannels(SignalDataPtr s);
	bool	InitSimulation(void);
	void	PrintInfoFields(void);
	bool	RunSimulation(void);
	bool	SetField(const wxString &name, bool value);
	bool	SetField(const wxString &name, short value);
	bool	SetField(const wxString &name, int value);
	bool	SetField(const wxString &name, long value);
	bool	SetField(const wxString &name, Float value);
	bool	SetField(const wxString &name, Float *values, int length);
	bool	SetInfoFields(SignalDataPtr s);
	bool	SetOutput(void);
	void	SetStringFromJUTF(wxString& s, jstring jstr);
	void	SetSimFileFromJUTF(jstring jstr) { SetStringFromJUTF(simFile, jstr); }
	void	SetParOptionsFromJUTF(jstring jstr) { SetStringFromJUTF(parOptions, jstr); }
	jobjectArray GetOutChannels(void)	{ return outChannels; }
	wxString&	GetSimFile(void)	{ return simFile; }
	wxString&	GetParOptions(void)	{ return parOptions; }
};

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

void	DPrint_LibRunDSAMSim(const WChar *format, va_list args);
void	Notify_LibRunDSAMSim(const wxChar *message, CommonDiagSpecifier type);

/******************************************************************************/
/*************************** Class Methods ************************************/
/******************************************************************************/

/*************************** Constructor **************************************/

LibRunDSAMSim::LibRunDSAMSim(JNIEnv *env, jclass cls)
{
	this->env = env;
	this->cls = cls;
	staticTimeFlag = false;
	numChannels = 0;
	interleaveLevel = 1;
	length = 0;
	inChannels = NULL;
	dt = 0.0;
	outputTimeOffset= 0.0;
	audModel = NULL;
	outSignal = NULL;
	inputSignal = NULL;
	mainApp = NULL;
}

/*************************** Destructor ***************************************/

LibRunDSAMSim::~LibRunDSAMSim(void)
{
	if (inputSignal)
		delete inputSignal;
	if (inChannels) {
		for (int i = 0; i < numChannels; i++)
			delete inChannels[i];
		delete inChannels;
	}
	if (mainApp)
		delete mainApp;
}

/*************************** SetSimFileFromUTF ********************************/

/*
 * Sets a string reference from a jstring.
 */

void
LibRunDSAMSim::SetStringFromJUTF(wxString& s, jstring jstr)
{
	const char *str = env->GetStringUTFChars(jstr, NULL);
	s = wxString::FromUTF8(str);
	env->ReleaseStringUTFChars(jstr, str);

}

/*************************** InitInputSignal **********************************/

/*
 * Initialise the input signal.
 */

bool
LibRunDSAMSim::InitInputSignal(jobjectArray inChannels)
{
	static const WChar *funcName = wxT("LibRunDSAMSim::InitSimulation");

	if (inChannels == NULL)
		return(true);
	numChannels = env->GetArrayLength(inChannels);
	length = env->GetArrayLength((jarray)env->GetObjectArrayElement(inChannels, 0));
	this->inChannels = new Float *[numChannels];
	for (int i = 0; i < numChannels; i++) {
		this->inChannels[i] = new Float [length];
#		if DSAM_USE_FLOAT
		jfloatArray jarr = (jfloatArray) env->GetObjectArrayElement(inChannels, i);
			env->GetFloatArrayRegion(jarr, 0, length, this->inChannels[i]);
#		else
			jdoubleArray jarr = (jdoubleArray) env->GetObjectArrayElement(inChannels, i);
			env->GetDoubleArrayRegion(jarr, 0, length, this->inChannels[i]);
#		endif
	}
	if ((inputSignal = new MatInSignal(this->inChannels, numChannels,
	  interleaveLevel, length, dt, staticTimeFlag, outputTimeOffset)) == NULL) {
		NotifyError(wxT("%s: Failed to initialise input signal."), funcName);
		return(false);
	}
	return(true);

}

/*************************** InitSimulation ***********************************/

/*
 * This function initialises the simulation ready for running.
 */

bool
LibRunDSAMSim::InitSimulation(void)
{
	static const WChar *funcName = wxT("LibRunDSAMSim::InitSimulation");

	if ((mainApp = new MatMainApp((WChar *) PROGRAM_NAME, simFile.c_str(),
	  parOptions.c_str(), inputSignal, false)) == NULL) {
		NotifyError(wxT("%s: Could not initialise the MatMainApp module."),
		  funcName);
		return(false);
	}
	return(true);

}

/*************************** RunSimulation *************************************/

/*
 * This function runs the simulqation and sets the process and outSignal pointers.
 */

bool
LibRunDSAMSim::RunSimulation(void)
{
	static const WChar *funcName = wxT("LibRunDSAMSim::RunSimulation");

	if (!mainApp->Main()) {
		NotifyError(wxT("%s: Could not run simulation."), funcName);
		return NULL;
	}
	audModel = mainApp->GetSimProcess();
	outSignal = _OutSig_EarObject(audModel);
	return(true);

}

/*************************** InitOutChannels **********************************/

/*
 * Set output data for return.
 */

bool
LibRunDSAMSim::InitOutChannels(SignalDataPtr s)
{
	static const WChar *funcName = wxT("LibRunDSAMSim::InitOutChannels");
	int		chan;
	jclass			doubleArrCls;

	if ((doubleArrCls = env->FindClass("[D")) == NULL) {
		NotifyError(wxT("%s: Could not allocate double array class."), funcName);
		return(false);
	}
	if ((outChannels = env->NewObjectArray(s->numChannels, doubleArrCls, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not allocate channel array."), funcName);
		return(false);
	}
	for (chan = 0; chan < s->numChannels; chan++) {
		jdoubleArray dArr = env->NewDoubleArray(s->length);
		if (dArr == NULL) {
			NotifyError(wxT("%s: Could not allocate channel [%d] memory."), funcName, chan);
			return(false);
		}
		env->SetDoubleArrayRegion(dArr, 0, s->length, s->channel[chan]);
		env->SetObjectArrayElement(outChannels, chan, dArr);
		env->DeleteLocalRef(dArr);
	}
	return(true);
}

/*************************** SetField **********************************/

/*
 * Sets a double field for the main class.
 */

bool
LibRunDSAMSim::SetField(const wxString &name, bool value)
{
	static const WChar *funcName = wxT("LibRunDSAMSim::SetField(bool)");
	jfieldID	fid;

	if ((fid = env->GetStaticFieldID(cls, name.utf8_str(), "Z")) == NULL) {
		NotifyError(wxT("%s: Could not find %s field ID."), funcName, name.c_str());
		return(false);
	}
	env->SetStaticBooleanField(cls, fid, value);
	return(true);
}

/*************************** SetField **********************************/

/*
 * Sets a double field for the main class.
 */

bool
LibRunDSAMSim::SetField(const wxString &name, int value)
{
	static const WChar *funcName = wxT("LibRunDSAMSim::SetField(int)");
	jfieldID	fid;

	if ((fid = env->GetStaticFieldID(cls, name.utf8_str(), "i")) == NULL) {
		NotifyError(wxT("%s: Could not find %s field ID."), funcName, name.c_str());
		return(false);
	}
	env->SetStaticIntField(cls, fid, value);
	return(true);
}

/*************************** SetField **********************************/

/*
 * Sets a double field for the main class.
 */

bool
LibRunDSAMSim::SetField(const wxString &name, short value)
{
	static const WChar *funcName = wxT("LibRunDSAMSim::SetField(short)");
	jfieldID	fid;

	if ((fid = env->GetStaticFieldID(cls, name.utf8_str(), "S")) == NULL) {
		NotifyError(wxT("%s: Could not find %s field ID."), funcName, name.c_str());
		return(false);
	}
	env->SetStaticShortField(cls, fid, value);
	return(true);
}

/*************************** SetField **********************************/

/*
 * Sets a double field for the main class.
 */

bool
LibRunDSAMSim::SetField(const wxString &name, long value)
{
	static const WChar *funcName = wxT("LibRunDSAMSim::SetField(long)");
	jfieldID	fid;

	if ((fid = env->GetStaticFieldID(cls, name.utf8_str(), "J")) == NULL) {
		NotifyError(wxT("%s: Could not find %s field ID."), funcName, name.c_str());
		return(false);
	}
	env->SetStaticLongField(cls, fid, value);
	return(true);
}

/*************************** SetField **********************************/

/*
 * Sets a double field for the main class.
 */

bool
LibRunDSAMSim::SetField(const wxString &name, Float value)
{
	static const WChar *funcName = wxT("LibRunDSAMSim::SetField(Float)");
	jfieldID	fid;

#	if DSAM_USE_FLOAT
		if ((fid = env->GetStaticFieldID(cls, name.utf8_str(), "F")) == NULL) {
			NotifyError(wxT("%s: Could not find %s field ID."), funcName, name.c_str());
			return(false);
		}
		env->SetStaticFloatField(cls, fid, value);
#	else
		if ((fid = env->GetStaticFieldID(cls, name.utf8_str(), "D")) == NULL) {
			NotifyError(wxT("%s: Could not find %s field ID."), funcName, name.c_str());
			return(false);
		}
		env->SetStaticDoubleField(cls, fid, value);
#	endif
	return(true);
}

/*************************** SetField **********************************/

/*
 * Sets a double field for the main class.
 */

bool
LibRunDSAMSim::SetField(const wxString &name, Float *values, int size)
{
	static const WChar *funcName = wxT("LibRunDSAMSim::SetField(Float *)");
	jfieldID	fid;

#	if DSAM_USE_FLOAT
		if ((fid = env->GetStaticFieldID(cls, name.utf8_str(), "[F")) == NULL) {
			NotifyError(wxT("%s: Could not find %s field ID."), funcName, name.c_str());
			return(false);
		}
		jdoubleArray dArr = env->NewFloatArray(size);
		if (fArr == NULL) {
			NotifyError(wxT("%s: Could not allocate array memory (%d)."), funcName, size);
			return(false);
		}
		env->SetFoatArrayRegion(dArr, 0, size, values);
		env->SetStaticObjectField(cls, fid, fArr);
#	else
		if ((fid = env->GetStaticFieldID(cls, name.utf8_str(), "[D")) == NULL) {
			NotifyError(wxT("%s: Could not find %s field ID."), funcName, name.c_str());
			return(false);
		}
		jdoubleArray dArr = env->NewDoubleArray(size);
		if (dArr == NULL) {
			NotifyError(wxT("%s: Could not allocate array memory (%d)."), funcName, size);
			return(false);
		}
		env->SetDoubleArrayRegion(dArr, 0, size, values);
		env->SetStaticObjectField(cls, fid, dArr);
#	endif
	return(true);
}

/*************************** SetInfoFields **********************************/

/*
 * Set output data for return.
 */

bool
LibRunDSAMSim::SetInfoFields(SignalDataPtr s)
{
	static const WChar *funcName = wxT("LibRunDSAMSim::SetInfoFields");
	jfieldID	fid;

	if (!SetField(wxT("dt"), s->dt))
		return(false);
	if (!SetField(wxT("staticTimeFlag"), (bool) s->staticTimeFlag))
		return(false);
	if (!SetField(wxT("numChannels"), (short) s->numChannels))
		return(false);
	if (!SetField(wxT("numWindowFrames"), (short) s->numWindowFrames))
		return(false);
	if (!SetField(wxT("outputTimeOffset"), s->outputTimeOffset))
		return(false);
	if (!SetField(wxT("interleaveLevel"), (short) s->interleaveLevel))
		return(false);
	if (!SetField(wxT("length"), (long) s->length))
		return(false);
	if (!SetField(wxT("labels"), s->info.chanLabel, (int) s->numChannels))
		return(false);
	return(true);
}

/*************************** SetOutput **************************************/

/*
 * This function sets the output signal and info parameters.
 */

bool
LibRunDSAMSim::SetOutput(void)
{
	static const WChar *funcName = wxT("LibRunDSAMSim::SetOutput");

	if (!InitOutChannels(outSignal)) {
		NotifyError(wxT("%s: Could not set output channels."), funcName);
		return(false);
	}
	if (!SetInfoFields(outSignal)) {
		NotifyError(wxT("%s: Could not set output info fields."), funcName);
		return(false);
	}
	return(true);
}

/*************************** GetBooleanField *******************************/

/*
 * Gets a double field from the main class.
 */

bool
LibRunDSAMSim::GetBooleanField(const wxString &name)
{
	static const WChar *funcName = wxT("LibRunDSAMSim::GetBooleanField");
	jfieldID	fid;

	fid = env->GetStaticFieldID(cls, name.utf8_str(), "Z");
	return(env->GetStaticBooleanField(cls, fid));
}

/*************************** GetIntField *******************************/

/*
 * Gets a double field from the main class.
 */

int
LibRunDSAMSim::GetIntField(const wxString &name)
{
	static const WChar *funcName = wxT("LibRunDSAMSim::GetIntField");
	jfieldID	fid;

	fid = env->GetStaticFieldID(cls, name.utf8_str(), "I");
	return(env->GetStaticIntField(cls, fid));
}

/*************************** GetShortField *******************************/

/*
 * Gets a double field from the main class.
 */

short
LibRunDSAMSim::GetShortField(const wxString &name)
{
	static const WChar *funcName = wxT("LibRunDSAMSim::GetShortField");
	jfieldID	fid;

	fid = env->GetStaticFieldID(cls, name.utf8_str(), "S");
	return(env->GetStaticShortField(cls, fid));
}

/*************************** GetLongField *******************************/

/*
 * Gets a double field from the main class.
 */

long
LibRunDSAMSim::GetLongField(const wxString &name)
{
	static const WChar *funcName = wxT("LibRunDSAMSim::GetLongField");
	jfieldID	fid;

	fid = env->GetStaticFieldID(cls, name.utf8_str(), "J");
	return(env->GetStaticLongField(cls, fid));
}

/*************************** GetFloatField *****************************/

/*
 * Gets a double field from the main class.
 */

Float
LibRunDSAMSim::GetFloatField(const wxString &name)
{
	static const WChar *funcName = wxT("LibRunDSAMSim::GetFloatField");
	jfieldID	fid;

#	if DSAM_USE_FLOAT
		fid = env->GetStaticFieldID(cls, name.utf8_str(), "F");
		return(env->GetStaticFloatField(cls, fid);
#	else
		fid = env->GetStaticFieldID(cls, name.utf8_str(), "D");
		return(env->GetStaticDoubleField(cls, fid));
#	endif
}

/*************************** GetInfoFields **********************************/

/*
 * Get info fields for signal settings.
 * At least the number of channels and sampling interval must be given.
 */

bool
LibRunDSAMSim::GetInfoFields(void)
{
	dt = GetFloatField(wxT("dt"));
	staticTimeFlag = GetBooleanField(wxT("staticTimeFlag"));
	numChannels = GetShortField(wxT("numChannels"));
	numWindowFrames = GetShortField(wxT("numWindowFrames"));
	outputTimeOffset = GetFloatField(wxT("outputTimeOffset"));
	interleaveLevel = GetShortField(wxT("interleaveLevel"));
	length = GetLongField(wxT("length"));
	return(true);
}

/*************************** PrintInfoFields **********************************/

/*
 * Print the info fields for debug purposes.
 */

void
LibRunDSAMSim::PrintInfoFields(void)
{
	wcout << wxT("dt = ") << dt << endl;
	wcout << wxT("staticTimeFlag = ") << staticTimeFlag << endl;
	wcout << wxT("numChannels = ") << numChannels << endl;
	wcout << wxT("numWindowFrames = ") << numWindowFrames << endl;
	wcout << wxT("outputTimeOffset = ") << outputTimeOffset << endl;
	wcout << wxT("interleaveLevel = ") << interleaveLevel << endl;
	wcout << wxT("length = ") << length << endl;
}

/******************************************************************************/
/*************************** Subroutines and functions ************************/
/******************************************************************************/

/*************************** DPrintStandard ***********************************/

/*
 * This routine prints out a diagnostic message, preceded by a bell sound.
 * It is used in the same way as the vprintf statement.
 * This is the standard version for ANSI C.
 */

void
DPrint_LibRunDSAMSim(const WChar *format, va_list args)
{
	CheckInitParsFile_Common();
	if (dSAM.parsFile == stdout) {
		wxString str, fmt;
		fmt = format;
		fmt.Replace(wxT("S"), wxT("s"));	/* This is very simplistic - requires revision */
		str.PrintfV(fmt, args);

		if (dSAM.diagnosticsPrefix)
			str = dSAM.diagnosticsPrefix + str;
		cout << str.utf8_str();
	} else {
		if (dSAM.diagnosticsPrefix)
			DSAM_fprintf(dSAM.parsFile, STR_FMT, dSAM.diagnosticsPrefix);
		DSAM_vfprintf(dSAM.parsFile, format, args);
	}

}

/***************************** Notify *****************************************/

/*
 * All notification messages are stored in the notification list.
 * This list is reset when the noficiationCount is zero.
 */

void
Notify_LibRunDSAMSim(const wxChar *message, CommonDiagSpecifier type)
{
	FILE	*fp;

	switch (type) {
	case COMMON_ERROR_DIAGNOSTIC:
		fp = GetDSAMPtr_Common()->errorsFile;
		break;
	case COMMON_WARNING_DIAGNOSTIC:
		fp = GetDSAMPtr_Common()->warningsFile;
		break;
	default:
		fp = stdout;
	}
	if (fp == stdout) {
		cout << wxConvUTF8.cWX2MB(message) << endl;
	} else {
		fprintf(fp, wxConvUTF8.cWX2MB(message));
		fprintf(fp, "\n");
	}

} /* Notify_LibRunDSAMSim */

/***************************** Java_RunDSAMSimJ_RunSimScript ***********************/

JNIEXPORT jobjectArray JNICALL
Java_RunDSAMSimJ_RunSimScriptCPP(JNIEnv *env, jclass cls, jstring jSimFile,
  jstring jParOptions, jobjectArray inChannels)
{
	LibRunDSAMSim	libRunDSAMSim(env, cls);

	SetDiagMode(COMMON_DIALOG_DIAG_MODE);
	SetErrorsFile_Common(wxT("screen"), OVERWRITE);
	SetNotifyFunc(Notify_LibRunDSAMSim);
	SetDPrintFunc(DPrint_LibRunDSAMSim);
	libRunDSAMSim.SetSimFileFromJUTF(jSimFile);
	libRunDSAMSim.SetParOptionsFromJUTF(jParOptions);

	if (!libRunDSAMSim.GetInfoFields()) {
		NotifyError(wxT("%s: Could not get input info fields."), PROGRAM_NAME);
		return(NULL);
	}
	//libRunDSAMSim.PrintInfoFields();
	libRunDSAMSim.InitInputSignal(inChannels);
	if (!libRunDSAMSim.InitSimulation()) {
		NotifyError(wxT("%s: Main application not initialised."), PROGRAM_NAME);
		return(NULL);
	}
	if (!libRunDSAMSim.RunSimulation()) {
		NotifyError(wxT("%s: Could not run simulation."), PROGRAM_NAME);
		return(NULL);
	}
	if (!libRunDSAMSim.SetOutput()) {
		NotifyError(wxT("%s: Could set output signal and information."), PROGRAM_NAME);
		return(NULL);
	}
	return libRunDSAMSim.GetOutChannels();

}
