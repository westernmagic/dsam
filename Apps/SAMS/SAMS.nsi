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

!addincludedir "..\dsam2830\NSIS"

;!include "LogicLib.nsh"
!include "Library.nsh"

!include "MUI2.nsh"		; Use Modern UI
!define	ALL_USERS		; For WriteEnvStr.nsh 
!include "WriteEnvStr.nsh"
!include "EnvVarUpdate.nsh"
!include "DSAMGenUtils.nsh"
!include "DSAMAppUtils.nsh"

!define APP_NAME		"AMS"
!define VERSION			"1.3.0"
!define INST_VERSION		"1"
!define MY_APP			"${APP_NAME}.exe"
!define MY_APP2			"${APP_NAME}_ng.exe"
!define APP_SHORTCUT		"${APP_NAME}.lnk"
!define APP_SHORTCUT2		"${APP_NAME}_ng.lnk"
!define REL32DIR		"Release (SDK)"
!define REL64DIR		"x64\Release"
!define APP_RKEY		"${DSAM_APP_RKEY}\${APP_NAME}"
!define README			"ReadMe"
!define README_FILE		"${README}.rtf"
!define README_SHORTCUT		"${README}.lnk"

;--------------------------------
; Global variables

;--------------------------------

; The name of the installer
Name "DSAM ${APP_NAME} ${VERSION}"

; The file to write
OutFile "${APP_NAME}_${VERSION}_${INST_VERSION}.exe"

; The default installation directory
; - PROGRAMFILEs64 translates to "Program Files" on 32-bit architecture.
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
	LicenseData LICENSE
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

  Push $0  
  DetailPrint "Platform architecture is $platformArch-bit"
  ; Sets the registry view for x64 systems.
  ${iF} $platformArch = 64
  	SetRegView 64
  ${EndIf}

  Push "${APP_RKEY}"
  Call CheckAppRegistration

  ; Write the installation path into the registry
  WriteRegStr HKLM "${APP_RKEY}" "Install_Dir" "$INSTDIR"

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

  Pop $0

SectionEnd

Section "Executable Files"

  SectionIn RO
  
  ; Set output path to the DLL directory for side-by-side installation
  Call SetDLLOutPath

  ; Put file there
  ${if} $platformArch = 32
    DetailPrint "Installing 32-bit Executable"
    File "MSVC\AMS\${REL32DIR}\${MY_APP}"	
    File "MSVC\AMS_ng\${REL32DIR}\${MY_APP2}"	
    CreateShortCut "$INSTDIR\${APP_SHORTCUT}" "${DLL32DIR}\${MY_APP}"
    CreateShortCut "$INSTDIR\${APP_SHORTCUT2}" "${DLL32DIR}\${MY_APP2}"
  ${else}
    DetailPrint "Installing 64-bit Executable"
    File MSVC\AMS\${REL64DIR}\${APP_NAME}.exe	
    File MSVC\AMS_ng\${REL64DIR}\${APP_NAME}_ng.exe	
    CreateShortCut "$INSTDIR\${APP_SHORTCUT}" "${DLL64DIR}\${MY_APP}"
    CreateShortCut "$INSTDIR\${APP_SHORTCUT2}" "${DLL64DIR}\${MY_APP2}"
  ${EndIf}
  
SectionEnd

Section "Support Libraries"
  SectionIn RO

  Call InstallMSVCRedist
  Call InstallSupportDLLs

SectionEnd

Section "Default Simulation Scripts"

  ; Set output path to the installation directory
  SetOutPath "$INSTDIR\DEFAULT"

  FILE /r /x *.am /x *.in "DEFAULT\*.*"

SectionEnd

Section "Tutorials"

  ; Set output path to the installation directory
  SetOutPath "$INSTDIR\Tutorials"

  FILE /r /x *.am /x *.in "Tutorials\*.*"

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

Section
	# this part is only necessary if you used /checknoshortcuts
	StrCpy $R1 $R0 1
	StrCmp $R1 ">" skip

		SetShellVarContext All
		CreateDirectory "$SMPROGRAMS\$R0"
		${iF} $platformArch = 32
			CreateShortCut "$SMPROGRAMS\$R0\${APP_SHORTCUT}" "${DLL32DIR}\${MY_APP}"
		${Else}
			CreateShortCut "$SMPROGRAMS\$R0\${APP_SHORTCUT}" "${DLL64DIR}\${MY_APP}"

		${EndIf}
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
    Delete ${DLL32DIR}\${MY_APP}
    Delete ${DLL32DIR}\${MY_APP2}
  ${Else}
    Delete ${DLL64DIR}\${MY_APP}
    Delete ${DLL64DIR}\${MY_APP2}
  ${EndIf}
  Delete $INSTDIR\${APP_SHORTCUT}
  Delete $INSTDIR\${APP_SHORTCUT2}
  Delete $INSTDIR\${README_FILE}
  Delete $INSTDIR\uninstall.exe

  ; Remove Start menu shortcuts
  SetShellVarContext All
  Delete "$SMPROGRAMS\$R0\${APP_SHORTCUT}"

  ; Remove DLLs
  Call un.SetDLLUnInstall
  Call un.SetInstallMSVCRedist

  ; Remove directories used
  RMDir /r /REBOOTOK "$INSTDIR\DEFAULT"
  RMDir /r /REBOOTOK "$INSTDIR\Tutorials"
  RMDir /REBOOTOK "$INSTDIR"	; Remove dir if empty
  RMDIR "$SMPROGRAMS\$R0"	; Remove dir if empty
  Pop $R0

SectionEnd

;--------------------------------

;Descriptions

  ;Language strings
#  LangString DESC_SecDummy ${LANG_ENGLISH} "A test section."

  ;Assign language strings to sections
#  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
#    !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
#  !insertmacro MUI_FUNCTION_DESCRIPTION_END

