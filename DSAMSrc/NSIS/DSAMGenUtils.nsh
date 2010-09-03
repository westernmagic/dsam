;;;;;;;;;;;;;;;;;;;;;;;
;File:		DSAMGenUtils.nsi
;Purpose:	General utility functions for DSAM Installers
;Comments:	
;Author:	L. P. O'Mard
;Created:	29 Jul 2010
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

!define DLL32DIR	"$COMMONFILES\dsam"
!define DLL64DIR	"$COMMONFILES64\dsam"
!define DSAMRKEY	"Software\DSAM"
!define DSAM_SDK_RKEY	"${DSAMRKEY}\SDK"
!define DSAM_APP_RKEY	"${DSAMRKEY}\Applications"
!define DSAM_APP_COUNT	"AppCount"
!define INSTALL_DIR	"Install_Dir"

!define WX_VERSION	"2.8.10"
!define WXWINDIR	"..\SupportLibs\wxWidgets-${WX_VERSION}"
!define LSF_VERSION	"1.0.17"
!define LIBSNDFILEDIR	"..\SupportLibs\libsndfile-${LSF_VERSION}"
!define PA_VERSION	"19"
!define PORTAUDIODIR	"..\SupportLibs\portaudio_v${PA_VERSION}"
!define FFTW_VERSION	"3.1.2"
!define FFTWDIR		"..\SupportLibs\fftw-${FFTW_VERSION}"

;--------------------------------
; Set PlatformArchitecture
; Makes available: platformArch

Section ; Hidden ensure that this function is called for installer

	Var /GLOBAL platformArch

	GetVersion::WindowsPlatformArchitecture
	Pop $platformArch
	${If} $platformArch = 64
		SetRegView 64
		!define LIBRARY_X64		; For InstallLib - can this be conditional?
	${EndIf}
	DetailPrint "Architecture is $platformArch-bit"

SectionEnd

###################################################################################
#
# Check Installed DLL
# Improved checks will look at version too
#
# Example:
#
# Push "DLL"
# Call CheckDLLStatus
# Pop "Status"
#

Function CheckDLLStatus

  Exch $0
  Push $1
  DetailPrint "Checking for '$0'"
  ${If} $platformArch = 32
    StrCpy $1 = "${DLL32DIR}"
  ${Else}
    StrCpy $1 = "${DLL64DIR}"
  ${EndIf}
  ${If} ${FileExists} "$1\$0"
    StrCpy $1 1
  ${Else}
    StrCpy $1 0
  ${EndIf}	
  DetailPrint "$0 installed flag: $1"
  Exch
  Pop $0
  Exch $1

FunctionEnd

###################################################################################
#
# Set the DLL output path.
#
# Example:
#
# Call SetDLLOutPath
#

Function SetDLLOutPath

  ; Insert DLL's
  ${If} $platformArch = 32
    SetOutPath ${DLL32DIR}
  ${Else}
    SetOutPath ${DLL64DIR}
  ${EndIf}

FunctionEnd

###################################################################################
#
# Install WxWin DLLs
#
# Example:
#
# Call InstallWxWinDLLs
#

Function InstallWxWinDLLs

  ; Insert DLL's
  Call SetDLLOutPath
  Push "wxbase28u_vc_custom.dll"
  Call CheckDLLStatus
  Pop $0

  ${If} $platformArch = 32
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
     ${WXWINDIR}\lib\vc_dll\wxbase28u_vc_custom.dll ${DLL32DIR}\wxbase28u_vc_custom.dll ${DLL32DIR}
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
     ${WXWINDIR}\lib\vc_dll\wxbase28u_net_vc_custom.dll ${DLL32DIR}\wxbase28u_net_vc_custom.dll ${DLL32DIR}
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
     ${WXWINDIR}\lib\vc_dll\wxbase28u_xml_vc_custom.dll ${DLL32DIR}\wxbase28u_xml_vc_custom.dll ${DLL32DIR}
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
     ${WXWINDIR}\lib\vc_dll\wxmsw28u_core_vc_custom.dll ${DLL32DIR}\wxmsw28u_core_vc_custom.dll ${DLL32DIR}
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
     ${WXWINDIR}\lib\vc_dll\wxmsw28u_html_vc_custom.dll ${DLL32DIR}\wxmsw28u_html_vc_custom.dll ${DLL32DIR}
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
     ${WXWINDIR}\lib\vc_dll\wxmsw28u_ogl_vc_custom.dll ${DLL32DIR}\wxmsw28u_ogl_vc_custom.dll ${DLL32DIR}
  ${Else}
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
     ${WXWINDIR}\lib64\vc_dll\wxbase28u_vc_custom.dll ${DLL64DIR}\wxbase28u_vc_custom.dll ${DLL64DIR}
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
     ${WXWINDIR}\lib64\vc_dll\wxbase28u_net_vc_custom.dll ${DLL64DIR}\wxbase28u_net_vc_custom.dll ${DLL64DIR}
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
     ${WXWINDIR}\lib64\vc_dll\wxbase28u_xml_vc_custom.dll ${DLL64DIR}\wxbase28u_xml_vc_custom.dll ${DLL64DIR}
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
     ${WXWINDIR}\lib64\vc_dll\wxmsw28u_core_vc_custom.dll ${DLL64DIR}\wxmsw28u_core_vc_custom.dll ${DLL64DIR}
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
     ${WXWINDIR}\lib64\vc_dll\wxmsw28u_html_vc_custom.dll ${DLL64DIR}\wxmsw28u_html_vc_custom.dll ${DLL64DIR}
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
     ${WXWINDIR}\lib64\vc_dll\wxmsw28u_ogl_vc_custom.dll ${DLL64DIR}\wxmsw28u_ogl_vc_custom.dll ${DLL64DIR}
  ${EndIf}

FunctionEnd

###################################################################################
#
# Install LibSndFile DLL
#
# Example:
#
# Call InstallLibSndFileDLL
#

Function InstallLibSndFileDLL

  ; Insert DLL's
  Call SetDLLOutPath
  ${If} $platformArch = 32
     Push "libsndfile_x86.dll"
  ${Else}
     Push "libsndfile_x64.dll"
  ${EndIf}
  Call CheckDLLStatus
  Pop $0

  ${If} $platformArch = 32
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
     ${LIBSNDFILEDIR}\MSVC\libsndfile\Release\libsndfile_x86.dll ${DLL32DIR}\libsndfile_x86.dll ${DLL32DIR}
  ${Else}
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
     ${LIBSNDFILEDIR}\MSVC\libsndfile\x64\Release\libsndfile_x64.dll ${DLL64DIR}\libsndfile_x64.dll ${DLL64DIR}
  ${EndIf}

FunctionEnd

###################################################################################
#
# Install PortAudio DLL
#
# Example:
#
# Call InstallPortAudioDLL
#

Function InstallPortAudioDLL

  Push $0
  ; Insert DLL's
  Call SetDLLOutPath
  Push "portaudio.dll"
  Call CheckDLLStatus
  Pop $0

  ${If} $platformArch = 32
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
     ${PORTAUDIODIR}\build\msvc\Win32\Release\portaudio.dll ${DLL32DIR}\portaudio.dll ${DLL32DIR}
  ${Else}
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
     ${PORTAUDIODIR}\build\msvc\x64\Release\portaudio.dll ${DLL64DIR}\portaudio.dll ${DLL64DIR}
  ${EndIf}

  Pop $0

FunctionEnd

###################################################################################
#
# Set Unintstall for DLLs
#
# Example:
#
# Call un.SetDLLUnInstall
#

Function un.SetDLLUnInstall

  Push $0
  ReadRegDWORD $0 HKLM ${DSAMRKEY} ${DSAM_APP_COUNT}
  ${If} $0 < 1
    ; Remove DLLs
    GetVersion::WindowsPlatformArchitecture
    Pop $platformArch 
    ${If} $platformArch  == 32
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL32DIR}\wxbase28u_net_vc_custom.dll
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL32DIR}\wxbase28u_vc_custom.dll
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL32DIR}\wxbase28u_xml_vc_custom.dll
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL32DIR}\wxmsw28u_core_vc_custom.dll
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL32DIR}\wxmsw28u_html_vc_custom.dll
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL32DIR}\wxmsw28u_ogl_vc_custom.dll
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL32DIR}\libsndfile_x86.dll
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL32DIR}\portaudio.dll
      RMDIR /REBOOTOK "${DLL32DIR}"
    ${Else}
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL64DIR}\wxbase28u_net_vc_custom.dll
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL64DIR}\wxbase28u_vc_custom.dll
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL64DIR}\wxbase28u_xml_vc_custom.dll
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL64DIR}\wxmsw28u_core_vc_custom.dll
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL64DIR}\wxmsw28u_html_vc_custom.dll
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL64DIR}\wxmsw28u_ogl_vc_custom.dll
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL64DIR}\libsndfile_x64.dll
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL64DIR}\portaudio.dll
      RMDIR /REBOOTOK "${DLL64DIR}"
    ${EndIf}
  ${EndIf}
  Pop $0

FunctionEnd
