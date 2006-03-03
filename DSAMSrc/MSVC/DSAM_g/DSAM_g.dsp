# Microsoft Developer Studio Project File - Name="DSAM_g" - Package Owner=<4> 
# Microsoft Developer Studio Generated Build File, Format Version 6.00 
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=DSAM_g - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DSAM_g.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DSAM_g.mak" CFG="DSAM_g - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DSAM_g - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DSAM_g - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DSAM_g - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "$(DSAMDIR)\include" /I "$(DSAMDIR)" /I "$(WXWIN)\include" /I "$(WXWIN)\lib\vc_dll\msw" /I "$(WXWIN)\contrib\include" /D "_MBCS" /D "_LIB" /D "NDEBUG" /D "GRAPHICS_SUPPORT" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D WXUSINGDLL=1 /D "WIN32" /D HAVE_CONFIG_H=1 /D LIBRARY_COMPILE=1 /D USE_WX_OGL=1 /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\DSAM_g.lib"

!ELSEIF  "$(CFG)" == "DSAM_g - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(DSAMDIR)\include" /I "$(DSAMDIR)" /I "$(WXWIN)\include" /I "$(WXWIN)\lib\vc_dll\msw" /I "$(WXWIN)\contrib\include" /D "_WINDOWS" /D "NDEBUG" /D "GRAPHICS_SUPPORT" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D WXUSINGDLL=1 /D "WIN32" /D HAVE_CONFIG_H=1 /D LIBRARY_COMPILE=1 /D USE_WX_OGL=1 /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /i "$(WXWIN)\include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\DSAM_g_d.lib"

!ENDIF 

# Begin Target

# Name "DSAM_g - Win32 Release"
# Name "DSAM_g - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\GrLib\GrAxisScale.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrBrushes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrCanvas.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrDiagFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrDialogInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrDisplayS.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrFonts.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrIPCServer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrLines.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrMainApp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrModParDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrParControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrParListInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrParListInfoList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrPrintDisp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrSDIBaseShapes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrSDICanvas.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrSDICommand.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrSDIDiagram.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrSDIDoc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrSDIDocManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrSDIEvtHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrSDIFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrSDIPalette.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrSDIShapes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrSDIView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrSDIXMLDoc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrSignalDisp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrSimMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\GrUtils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resources"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\src\GrLib\wx\msw\blank.cur
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\msw\bullseye.cur
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\msw\error.ico
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\msw\hand.cur
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\msw\info.ico
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\msw\magnif1.cur
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\msw\noentry.cur
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\msw\pbrush.cur
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\msw\pencil.cur
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\msw\pntleft.cur
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\msw\pntright.cur
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\msw\query.cur
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\msw\question.ico
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\msw\roller.cur
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\msw\size.cur
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\msw\tip.ico
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\msw\warning.ico
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\msw\watch1.cur
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\html\msw\wbook.ico
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\html\msw\wfolder.ico
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\html\msw\whelp.ico
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\html\msw\whlproot.ico
# End Source File
# Begin Source File

SOURCE=..\..\src\GrLib\wx\html\msw\wpage.ico
# End Source File
# End Group
# End Target
# End Project
