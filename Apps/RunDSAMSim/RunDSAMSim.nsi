;;;;;;;;;;;;;;;;;;;;;;;
;File:		AMS.nsi
;Purpose:	Installer for the AMS application
;Comments:	This uses the GetVersion plugin to determine the windows platform (32/64 bit?)
;Author:	L. P. O'Mard
;Created:	30 Jul 2010
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

!addincludedir "..\dsam2844\NSIS"

;!include "LogicLib.nsh"
!include "Library.nsh"

!include "MUI2.nsh"		; Use Modern UI
!define	ALL_USERS		; For WriteEnvStr.nsh 
!include "WriteEnvStr.nsh"
!include "EnvVarUpdate.nsh"
!include "DSAMGenUtils.nsh"
!include "DSAMAppUtils.nsh"

!define APP_NAME		"RunDSAMSim"
!define VERSION			"2.1.8"
!define INST_VERSION		"1"
!define MATLIB			"MatLib"
!define MATLIBDLL		"${MATLIB}.dll"
!define JAVALIB			"Lib${APP_NAME}"
!define JAVALIBDLL		"${JAVALIB}.dll"
!define GENLIB			"Lib${APP_NAME}Gen"
!define GENLIBDLL		"${GENLIB}.dll"
!define JARPGK			"${APP_NAME}jni-${VERSION}.jar"
!define REL32DIR		"DLL Release"
!define REL64DIR		"x64\DLL Release"
!define APP_RKEY		"${DSAM_APP_RKEY}\${APP_NAME}"
!define README			"ReadMe"
!define README_FILE		"${README}.rtf"
!define README_SHORTCUT		"${README}.lnk"
!define PYTHON_MODULE_PATH	"$INSTDIR\python"

;--------------------------------
; Global variables

;--------------------------------


; The name of the installer
Name "DSAM ${App_NAME} ${VERSION}"

; The file to write
OutFile "${APP_NAME}_${VERSION}_${INST_VERSION}.exe"

; The default installation directory
InstallDir "$PROGRAMFILES64\${COMPANY_DIR}\${APP_NAME}"

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "${APP_RKEY}" "${INSTALL_DIR}"	

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
; Pages

; License page - replaces: Page license
PageEx license
	LicenseText "DSAM ${APP_NAME} License"
	LicenseData COPYING
PageExEnd

Page components
Page directory
Page custom StartMenuGroupSelect "" ": Start Menu Folder"
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

; The stuff to install
Section "${APP_NAME} ${VERSION}"

  SectionIn RO
  
  DetailPrint "Platform architecture is $platformArch-bit"
  ; Sets the registry view for x64 systems.
  ${iF} $platformArch = 64
  	SetRegView 64
  ${EndIf}

  Push "${APP_RKEY}"
  Call CheckAppRegistration

  ; Write the installation path into the registry
  WriteRegStr HKLM "${APP_RKEY}" "Install_Dir" "$INSTDIR"

  ; Check for installed applications
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayName" "DSAM ${APP_NAME} ${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "NoRepair" 1

  ; Set output path to the install directory.
  SetOutPath $INSTDIR
  File ${README_FILE}
  WriteUninstaller "uninstall.exe"

  ; Set Environment variable
  Push "${APP_NAME}"
  Push "$INSTDIR"
  Call WriteEnvStr

 ; Update environment PATH variable
  Call SetEnvPathVar

SectionEnd

Section "MatLib DLL"

  SectionIn RO
  
  ; Set output path to the DLL directory for side-by-side installation
  Call SetDLLOutPath

  ; Put files there
  ${if} $platformArch = 32
    DetailPrint "Installing 32-bit DLL Support"
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
      "MSVC\${MATLIB}\${REL32DIR}\${MATLIBDLL}" "${DLL32DIR}\${MATLIBDLL}" "${DLL32DIR}"
  ${else}
    DetailPrint "Installing 64-bit DLL Support"
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
      "MSVC\${MATLIB}\${REL64DIR}\${MATLIBDLL}" "${DLL64DIR}\${MATLIBDLL}" "${DLL64DIR}"
  ${EndIf}
  
SectionEnd

Section "Java interface"
  
  ; Set output path to the DLL directory for side-by-side installation
  Call SetDLLOutPath

  ; Put files there
  ${if} $platformArch = 32
    DetailPrint "Installing 32-bit DLL Support"
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
      "MSVC\${JAVALIB}\${REL32DIR}\${JAVALIBDLL}" "${DLL32DIR}\${JAVALIBDLL}" "${DLL32DIR}"
  ${else}
    DetailPrint "Installing 64-bit DLL Support"
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
      "MSVC\${JAVALIB}\${REL64DIR}\${JAVALIBDLL}" "${DLL64DIR}\${JAVALIBDLL}" "${DLL64DIR}"
  ${EndIf}
  
  File src\java\${JARPGK}

SectionEnd

Section "Python interface"
  
  ; Set output path to the DLL directory for side-by-side installation
  Call SetDLLOutPath

  ; Put files there
  ${if} $platformArch = 32
    DetailPrint "Installing 32-bit DLL Support"
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
      "MSVC\${GENLIB}\${REL32DIR}\${GENLIBDLL}" "${DLL32DIR}\${GENLIBDLL}" "${DLL32DIR}"
  ${else}
    DetailPrint "Installing 64-bit DLL Support"
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
      "MSVC\${GENLIB}\${REL64DIR}\${GENLIBDLL}" "${DLL64DIR}\${GENLIBDLL}" "${DLL64DIR}"
  ${EndIf}
  
  ; Set output path to the installation directory.
  SetOutPath "${PYTHON_MODULE_PATH}\dsam"
  File src\python\dsam\*.py

  ; Set Environment variable
  Push "${PYTHON_MODULE_PATH}"
  Call SetEnvPythonPathVar

SectionEnd

Section "Support Libraries"
  SectionIn RO

  Call InstallMSVCRedist
  Call InstallSupportDLLs

SectionEnd

Section "Examples"

  ; Set output path to the installation directory.
  SetOutPath "$INSTDIR\Examples"
  File Examples\*

SectionEnd

;--------------------------------
; StartMenu page callbacks Sections

Function StartMenuGroupSelect

	Push $R1
	StartMenu::Select /checknoshortcuts "Don't create a start menu folder" \
	  /autoadd /lastused "${COMPANY_DIR}" "${APP_NAME} Application"
	Pop $R1

	StrCmp $R1 "success" success
	StrCmp $R1 "cancel" done
		; error
		MessageBox MB_OK $R1
		StrCpy $R0 "${APP_NAME} Application" # use default
		Return
	success:
	Pop $R0

	done:
	Pop $R1
FunctionEnd

Section ; hidden section
	# this part is only necessary if you used /checknoshortcuts
	StrCpy $R1 $R0 1
	StrCmp $R1 ">" skip

		SetShellVarContext All
		CreateDirectory "$SMPROGRAMS\$R0"
		CreateShortCut "$SMPROGRAMS\$R0\${README_SHORTCUT}" "$INSTDIR\${README_FILE}"
		; Write the installation path into the registry
		${iF} $platformArch = 64
			SetRegView 64
		${EndIf}
		WriteRegStr HKLM "${APP_RKEY}" "${START_MENU_GROUP}" "$R0"
	skip:
SectionEnd

;--------------------------------
; Callbacks Functions
;--------------------------------

Function .onVerifyInstDir
	IfFileExists "$INSTDIR\uninstall.exe" do_warn skip
	do_warn:	MessageBox MB_OK "${APP_NAME} is  already installed"
			Abort
	skip:
FunctionEnd

Function .onInstSuccess
	MessageBox MB_YESNO "View ${README_FILE} now (recommended)?" IDNO NoReadme
		ExecShell "open" "$INSTDIR\${README_FILE}"
	NoReadme:
FunctionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
 
  GetVersion::WindowsPlatformArchitecture
  Pop $platformArch
  ; Sets the registry view for x64 systems.
  ${If} $platformArch = 64
  	SetRegView 64
  ${EndIf}

  Push $R0
  ReadRegDWORD $R0 HKLM ${APP_RKEY} "${START_MENU_GROUP}"

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
  DeleteRegKey HKLM ${APP_RKEY}

  ; Remove environment variables
  # remove the variable
  Push "${APP_NAME}"
  Call un.DeleteEnvStr
  Call Un.SetEnvPathVar

  ; Decrement DSAM Application count
  Push "${APP_NAME}"
  call un.UpdateDSAMAppCount

  ; Remove files and uninstaller
  ${If} $platformArch = 32
    Delete ${DLL32DIR}\${JARPGK}
  ${Else}
    Delete ${DLL64DIR}\${JARPGK}
  ${EndIf}
  Delete $INSTDIR\${README_FILE}
  Delete $INSTDIR\uninstall.exe

  ; Remove Start menu shortcuts
  SetShellVarContext All
  Delete "$SMPROGRAMS\$R0\${README_SHORTCUT}"

  ; Remove DLLs
  ${If} $platformArch = 32
   !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED "${DLL32DIR}\${MATLIBDLL}"
   !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED "${DLL32DIR}\${JAVALIBDLL}"
   !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED "${DLL32DIR}\${GENLIBDLL}"
  ${Else}
   !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED "${DLL64DIR}\${MATLIBDLL}"
   !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED "${DLL64DIR}\${JAVALIBDLL}"
   !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED "${DLL64DIR}\${GENLIBDLL}"
  ${EndIf}
  Call un.SetDLLUnInstall
  Call un.SetInstallMSVCRedist

  ; Remove directories used
  RMDir /r /REBOOTOK "$INSTDIR\Examples"
  RMDir /REBOOTOK "$INSTDIR"	; Remove dir if empty

  ; Remove environment variables
  # remove the python path variable
  Push "${PYTHON_MODULE_PATH}"
  Call un.SetEnvPythonPathVar
 
SectionEnd

;--------------------------------

;Descriptions

  ;Language strings
#  LangString DESC_SecDummy ${LANG_ENGLISH} "A test section."

  ;Assign language strings to sections
#  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
#    !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
#  !insertmacro MUI_FUNCTION_DESCRIPTION_END

