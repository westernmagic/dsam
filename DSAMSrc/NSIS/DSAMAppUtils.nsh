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
