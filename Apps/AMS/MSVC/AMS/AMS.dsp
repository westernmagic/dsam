# Microsoft Developer Studio Project File - Name="AMS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AMS - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AMS.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AMS.mak" CFG="AMS - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AMS - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AMS - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AMS - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "$(DSAMDIR)/include" /I "$(WXWIN)/include" /I "$(DSAMDIR)" /I "$(WXWIN)\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D HAVE_CONFIG_H=1 /D "GRAPHICS_SUPPORT" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D WXUSINGDLL=1 /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /i "$(DSAMDIR)/include" /i "$(WXWIN)/include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib DSAM.lib DSAM_g.lib wx22_7.lib /nologo /subsystem:windows /machine:I386 /libpath:"$(WXWIN)/lib" /libpath:"$(DSAMDIR)\lib" /libpath:"$(WXWIN)\lib"

!ELSEIF  "$(CFG)" == "AMS - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(DSAMDIR)\include" /I "$(WXWIN)\include" /I "$(DSAMDIR)" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D HAVE_CONFIG_H=1 /D WXUSINGDLL=1 /D "GRAPHICS_SUPPORT" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /i "$(DSAMDIR)\src\GrLib" /i "$(WXWIN)\include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wx22_7.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"$(DSAMDIR)\lib" /libpath:"$(WXWIN)\lib"

!ENDIF 

# Begin Target

# Name "AMS - Win32 Release"
# Name "AMS - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\ams.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\ams.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\ams.rc
# End Source File
# Begin Source File

SOURCE=..\..\amsicon.ico
# End Source File
# Begin Source File

SOURCE=..\..\wx\msw\blank.cur
# End Source File
# Begin Source File

SOURCE=..\..\wx\msw\bullseye.cur
# End Source File
# Begin Source File

SOURCE=..\..\wx\msw\error.ico
# End Source File
# Begin Source File

SOURCE=..\..\wx\msw\hand.cur
# End Source File
# Begin Source File

SOURCE=..\..\wx\msw\info.ico
# End Source File
# Begin Source File

SOURCE=..\..\wx\msw\magnif1.cur
# End Source File
# Begin Source File

SOURCE=..\..\wx\msw\noentry.cur
# End Source File
# Begin Source File

SOURCE=..\..\wx\msw\pbrush.cur
# End Source File
# Begin Source File

SOURCE=..\..\wx\msw\pencil.cur
# End Source File
# Begin Source File

SOURCE=..\..\wx\msw\pntleft.cur
# End Source File
# Begin Source File

SOURCE=..\..\wx\msw\pntright.cur
# End Source File
# Begin Source File

SOURCE=..\..\wx\msw\query.cur
# End Source File
# Begin Source File

SOURCE=..\..\wx\msw\question.ico
# End Source File
# Begin Source File

SOURCE=..\..\wx\msw\roller.cur
# End Source File
# Begin Source File

SOURCE=..\..\wx\msw\size.cur
# End Source File
# Begin Source File

SOURCE=..\..\wx\msw\tip.ico
# End Source File
# Begin Source File

SOURCE=..\..\wx\msw\warning.ico
# End Source File
# Begin Source File

SOURCE=..\..\wx\msw\watch1.cur
# End Source File
# Begin Source File

SOURCE=..\..\wx\html\msw\wbook.ico
# End Source File
# Begin Source File

SOURCE=..\..\wx\html\msw\wfolder.ico
# End Source File
# Begin Source File

SOURCE=..\..\wx\html\msw\whelp.ico
# End Source File
# Begin Source File

SOURCE=..\..\wx\html\msw\whlproot.ico
# End Source File
# Begin Source File

SOURCE=..\..\wx\html\msw\wpage.ico
# End Source File
# End Group
# End Target
# End Project
