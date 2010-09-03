;;;;;;;;;;;;;;;;;;;;;;;
;File:		DSAMAppUtils.nsi
;Purpose:	Application utility functions for DSAM Installers
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

!define COMPANY_DIR	"DSAM Applications"
!define START_MENU_GROUP	"StartMenuGroup"
!define MSVC_REDIST_DIR	"C:\Program Files\Microsoft Visual Studio 8\VC\redist"

###################################################################################
#
# Check Application registration
# 
#
# Example:
#
# Push "${AppRKey}"		# Application Registory Key
#
# Call CheckAppRegistration

Function CheckAppRegistration

  Exch $0
  Push $1

  ReadRegStr $1 HKLM $0 ${INSTALL_DIR}
  ${If} $1 == ""
    ReadRegDWORD $1 HKLM ${DSAMRKEY} ${DSAM_APP_COUNT}
    ${If} $1 < 1
      DetailPrint "DSAM Application count being created"
      WriteRegStr HKLM ${DSAMRKEY} ${DSAM_APP_COUNT} 1
    ${Else}
      DetailPrint "DSAM Application count being incremented"
      IntOp $1 $1 + 1
      WriteRegStr HKLM ${DSAMRKEY} ${DSAM_APP_COUNT} $1
    ${EndIf}
  ${EndIf}

  Pop $1
  Pop $0

FunctionEnd

###################################################################################
#
# Install Microsoft Visual Studio Redistributable support.
# 
#
# Example:
#
# Call InstallMSVCRedist

Function InstallMSVCRedist

  Push $0

  ${if} $platformArch = 32
    DetailPrint "Installing 32-bit MSVCRT Support"
    SetOutPath ${DLL32DIR}
    Push "msvcr80.dll"
    Call CheckDLLStatus
    Pop $0
     !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
      "${MSVC_REDIST_DIR}\x86\Microsoft.VC80.CRT\Microsoft.VC80.CRT.manifest" \
      ${DLL32DIR}\Microsoft.VC80.CRT.manifest ${DLL32DIR}  
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
      "${MSVC_REDIST_DIR}\x86\Microsoft.VC80.CRT\msvcp80.dll" ${DLL32DIR}\msvcp80.dll ${DLL32DIR}  
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
      "${MSVC_REDIST_DIR}\x86\Microsoft.VC80.CRT\msvcr80.dll" ${DLL32DIR}\msvcr80.dll ${DLL32DIR}  
  ${Else}
    DetailPrint "Installing 64-bit MSVCRT Support"
    SetOutPath ${DLL64DIR}
    Push "msvcr80.dll"
    Call CheckDLLStatus
    Pop $0
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
      "${MSVC_REDIST_DIR}\amd64\Microsoft.VC80.CRT\Microsoft.VC80.CRT.manifest" \
      ${DLL64DIR}\Microsoft.VC80.CRT.manifest ${DLL64DIR}  
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
      "${MSVC_REDIST_DIR}\amd64\Microsoft.VC80.CRT\msvcp80.dll" ${DLL64DIR}\msvcp80.dll ${DLL64DIR}  
    !insertmacro InstallLib DLL $0 REBOOT_NOTPROTECTED \
      "${MSVC_REDIST_DIR}\amd64\Microsoft.VC80.CRT\msvcr80.dll" ${DLL64DIR}\msvcr80.dll ${DLL64DIR}  
  ${EndIf}

  Pop $0

FunctionEnd

###################################################################################
#
# Uninstall Microsoft Visual Studio Redistributable support.
#
# Example:
#
# Call un.SetInstallMSVCRedist
#

Function un.SetInstallMSVCRedist

  Push $0
  Push $R0

  ReadRegDWORD $0 HKLM ${DSAMRKEY} ${DSAM_APP_COUNT}
  ${If} $0 < 1
    ; Remove DLLs
    GetVersion::WindowsPlatformArchitecture
    Pop $R0
    ${If} $R0 = 32
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL32DIR}\Microsoft.VC80.CRT.manifest
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL32DIR}\msvcp80.dll
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL32DIR}\msvcr80.dll
    ${Else}
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL64DIR}\Microsoft.VC80.CRT.manifest
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL64DIR}\msvcp80.dll
      !insertmacro UninstallLib DLL SHARED REBOOT_NOTPROTECTED ${DLL64DIR}\msvcr80.dll
    ${EndIf}
  ${EndIf}

  Pop $R0
  Pop $0

FunctionEnd

###################################################################################
#
# Install support DLLs
#
# Example:
#
# Call InstallSupportDLLs
#

Function InstallSupportDLLs

  Call InstallWxWinDLLs
  Call InstallLibSndFileDLL
  Call InstallPortAudioDLL

FunctionEnd

###################################################################################
#
# This function updates the registery DSaM application count
# 
#
# Example:
#
# Call un.UpdateDSAMAppCount 

Function un.UpdateDSAMAppCount 

  Push $0
  ReadRegDWORD $0 HKLM ${DSAMRKEY} ${DSAM_APP_COUNT}
  ${If} $0 < 2
    DeleteRegValue HKLM ${DSAMRKEY} ${DSAM_APP_COUNT}
    ReadRegStr $0 HKLM ${DSAM_SDK_RKEY} ${INSTALL_DIR}
    ${If} $0 == ""
      DetailPrint "Last DSAM Application, SDK not installed, removing DSAM Registry key"
      DeleteRegKey HKLM ${DSAMRKEY}
    ${EndIf}
  ${Else}
    IntOp $0 $0 - 1
    WriteRegStr HKLM ${DSAMRKEY} ${DSAM_APP_COUNT} $0
  ${EndIf}
  Pop $0

FunctionEnd

###################################################################################
#
# Set the PATH environment variable
#
# Example:
#
# Call SetEnvPathVar
#

Function SetEnvPathVar

  GetVersion::WindowsPlatformArchitecture
  Pop $platformArch 
  ${iF} $platformArch = 32
  	${EnvVarUpdate} $0 "PATH" "A" "HKLM" "${DLL32DIR}" ; Append  
  ${Else}
  	${EnvVarUpdate} $0 "PATH" "A" "HKLM" "${DLL64DIR}" ; Append  
  ${EndIf}
 
FunctionEnd

###################################################################################
#
# Unset the PATH environment variable
#
# Example:
#
# Call un.SetEnvPathVar
#

Function un.SetEnvPathVar

  Push $0
  ReadRegDWORD $0 HKLM ${DSAMRKEY} ${DSAM_APP_COUNT}
  ${If} $0 < 1
    ; Remove DLLs
    GetVersion::WindowsPlatformArchitecture
    Pop $platformArch 
    ${iF} $platformArch = 32
      ${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "${DLL32DIR}"
    ${Else}
      ${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "${DLL64DIR}"  
    ${EndIf}
  ${EndIf}
  Pop $0

FunctionEnd
