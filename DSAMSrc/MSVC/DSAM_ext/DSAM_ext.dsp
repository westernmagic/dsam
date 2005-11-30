# Microsoft Developer Studio Project File - Name="DSAM_ext" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=DSAM_ext - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DSAM_ext.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DSAM_ext.mak" CFG="DSAM_ext - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DSAM_ext - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DSAM_ext - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DSAM_ext - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(DSAMDIR)\include" /I "$(DSAMDIR)" /I "$(WXWIN)\include" /I "$(WXWIN)\lib\vc_dll\msw" /I "$(WXWIN)\contrib\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D HAVE_CONFIG_H=1 /D LIBRARY_COMPILE=1 /D WXUSINGDLL=1 /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\DSAM_ext.lib"

!ELSEIF  "$(CFG)" == "DSAM_ext - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(DSAMDIR)\include" /I "$(DSAMDIR)" /I "$(WXWIN)\include" /I "$(WXWIN)\lib\vc_dll\msw" /I "$(WXWIN)\contrib\include" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "WIN32" /D HAVE_CONFIG_H=1 /D LIBRARY_COMPILE=1 /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\DSAM_ext_d.lib"

!ENDIF 

# Begin Target

# Name "DSAM_ext - Win32 Release"
# Name "DSAM_ext - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\ExtLib\ExtIPCClient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ExtLib\ExtIPCServer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ExtLib\ExtIPCUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ExtLib\ExtMainApp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ExtLib\ExtProcThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ExtLib\ExtRunThreadedProc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ExtLib\ExtSingleProcThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ExtLib\ExtProcChainThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ExtLib\ExtSimThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ExtLib\ExtSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ExtLib\ExtSocketServer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ExtLib\ExtXMLDocument.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ExtLib\tinystr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ExtLib\tinyxml.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ExtLib\tinyxmlerror.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ExtLib\tinyxmlparser.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
