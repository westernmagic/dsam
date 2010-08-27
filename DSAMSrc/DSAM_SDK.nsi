;;;;;;;;;;;;;;;;;;;;;;;
;File:		DSAM_SDK.nsi
;Purpose:	Create the DSAM SDK required to compile DSAM programs
;Comments:	This script is based on NSIS example2.nsi
;			The "SetOutPath" is needed for the writing of the uninstaller and
;			and also the DLL install - even though the full directory is given
;			in the instruction line.
;Author:	L. P. O'Mard
;Created:	9 Mar 2009
;Updated:
;Copyright:	(c) 2010 Lowel P. O'Mard
;
;;;;;;;;;;;;;;;;;;;;;;;
;
;  This file is part of DSAM.
;
;  DSAM is free software: you can redistribute it and/or modify
;  it under the terms of the GNU General Public License as published by
;  the Free Software Foundation, either version 3 of the License, or
;  (at your option) any later version.
;
;  DSAM is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License
;  along with DSAM.  If not, see <http://www.gnu.org/licenses/>.
;
;;;;;;;;;;;;;;;;;;;;;;;

!addincludedir "NSIS"
;!include "LogicLib.nsh"
!include "Library.nsh"

!include "MUI2.nsh"		; Use Modern UI
!define	ALL_USERS		; For WriteEnvStr.nsh 
!include "WriteEnvStr.nsh"
!include "DSAMGenUtils.nsh"

!define VERSION		"2.8.29"
!define INST_VERSION	"1"
!define DSAMDIR_ENV	"DSAMSDKDIR"

;--------------------------------

; The name of the installer
Name "DSAM SDK ${VERSION}"

; The file to write
OutFile "DSAM_SDK_${VERSION}_${INST_VERSION}.exe"

; The default installation directory
; Currently the SDK has to be installed to the c drive to avoid
; file names with spaces for the "mex" compile command.
InstallDir C:\DSAM\SDKs

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM ${DSAM_SDK_RKEY} ${INSTALL_DIR}

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------

; Pages

  !insertmacro MUI_PAGE_LICENSE "COPYING"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

; The stuff to install
Section "DSAM SDK ${VERSION}"

  SectionIn RO

  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\DSAM\SDK "Install_Dir" "$INSTDIR"

  ; Check for installed applications
  
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DSAM_SDK" "DisplayName" "DSAM SDK ${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DSAM_SDK" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DSAM_SDK" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DSAM_SDK" "NoRepair" 1
  ; Set output path to the install directory.
  SetOutPath $INSTDIR
  WriteUninstaller "uninstall.exe"

SectionEnd

Section "DSAM Library ${VERSION} x86"

  SectionIn RO
  
  ; Set output path to the include directory.
  SetOutPath $INSTDIR\include\dsam

  ; Put file there
  File Include\*.h
  File DSAMSetup.h

  ; Set output path to the lib directory.
  SetOutPath $INSTDIR\lib\x86
  File lib\*x86.lib

  ; Set Environment variable
  Push "${DSAMDIR_ENV}"
  Push "$INSTDIR"
  Call WriteEnvStr
  
SectionEnd

Section "WXWIN Library ${WX_VERSION} x86"
  
  SectionIn RO

  ; Set output path to the include directory.
  SetOutPath $INSTDIR\include\wxwin

  ; Put file there
  File /r ${WXWINDIR}\Include\*.h
  File /r ${WXWINDIR}\contrib\Include\*.h
  File /r ${WXWINDIR}\lib\vc_dll\mswu\*.h

  ; Set output path to the lib directory.
  SetOutPath $INSTDIR\lib\x86
  File ${WXWINDIR}\lib\vc_dll\*.lib
  
  Call InstallWxWinDLLs

SectionEnd

Section "LIBSNDFILE Library ${LSF_VERSION} x86"
  
  SectionIn RO

  ; Set output path to the include directory.
  SetOutPath $INSTDIR\include\libsndfile

  ; Put file there
  File /r ${LIBSNDFILEDIR}\src\*.h

  ; Set output path to the lib directory.
  SetOutPath $INSTDIR\lib\x86
  File ${LIBSNDFILEDIR}\MSVC\libsndfile\Release\libsndfile_x86.lib

  Call InstallLibSndFileDLL
  
SectionEnd

Section "PORTAUDIO Library ${PA_VERSION} x86"
  
  SectionIn RO

  ; Set output path to the include directory.
  SetOutPath $INSTDIR\include\portaudio

  ; Put file there
  File /r ${PORTAUDIODIR}\include\*.h

  ; Set output path to the lib directory.
  SetOutPath $INSTDIR\lib\x86
  File ${PORTAUDIODIR}\build\msvc\Win32\Release\*.lib

  Call InstallPortAudioDLL

SectionEnd

Section "FFTW Library ${FFTW_VERSION} x86"
  
  SectionIn RO

  ; Set output path to the include directory.
  SetOutPath $INSTDIR\include\fftw

  ; Put file there
  File /r ${FFTWDIR}\api\*.h

  ; Set output path to the lib directory.
  SetOutPath $INSTDIR\lib\x86
  File ${FFTWDIR}\libfftw.lib

SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DSAM_SDK"
  DeleteRegKey HKLM ${DSAM_SDK_RKEY}

  ; Remove environment variables
  # remove the variable
  Push "${DSAMDIR_ENV}"
  Call un.DeleteEnvStr

  ; Remove files and uninstaller
  RMDIR /r /REBOOTOK $INSTDIR\include
  RMDIR /r /REBOOTOK $INSTDIR\lib\x86
  Delete $INSTDIR\uninstall.exe

  ; Remove DLLs
  Call un.SetDLLUnInstall

  ; Remove directories used
  RMDir /REBOOTOK "$INSTDIR" ; - remove if empty.

SectionEnd

;--------------------------------

;Descriptions

  ;Language strings
  LangString DESC_SecDummy ${LANG_ENGLISH} "A test section."

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END


