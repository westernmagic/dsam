# Microsoft Developer Studio Project File - Name="AMS_ng" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=AMS_ng - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AMS_ng.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AMS_ng.mak" CFG="AMS_ng - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AMS_ng - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "AMS_ng - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AMS_ng - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I "$(WXWIN)\include" /I "$(WXWIN)\lib\vc_dll\msw" /I "." /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /Fd"vc_mswdll\console.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I "$(DSAMDIR)/include" /I "$(WXWIN)\include" /I "$(WXWIN)\lib\vc_dll\msw" /I "." /I "$(DSAMDIR)" /I "$(WXWIN)/include" /I "$(PORTAUDIO)/pa_common" /D "__WXMSW__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /D "EXTENSIONS_SUPPORT" /D HAVE_CONFIG_H=1 /D "WIN32" /Fd"vc_mswdll\console.pdb" /FD /EHsc /c
# ADD BASE RSC /l 0x409 /i "$(WXWIN)\include" /i "$(WXWIN)\lib\vc_dll\msw" /i "." /d "__WXMSW__" /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /i "$(WXWIN)\include" /i "$(WXWIN)\lib\vc_dll\msw" /i "." /d "__WXMSW__" /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase26_net.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:console /machine:I386 /libpath:"$(WXWIN)\lib\vc_dll"
# ADD LINK32 DSAM_std.lib DSAM.lib DSAM_ext.lib wxbase26_net.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib winmm.lib comctl32.lib rpcrt4.lib wsock32.lib oleacc.lib portaudio.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /nologo /subsystem:console /machine:I386 /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"$(DSAMDIR)\lib" /libpath:"$(PORTAUDIO)\pa_win\msvc"

!ELSEIF  "$(CFG)" == "AMS_ng - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(DSAMDIR)\include" /I "$(DSAMDIR)" /I "$(WXWIN)/include" /I "$(WXWIN)\lib\vc_dll\msw" /I "$(PORTAUDIO)/pa_common" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D HAVE_CONFIG_H=1 /D "WIN32" /D "EXTENSIONS_SUPPORT" /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 DSAM_d.lib DSAM_std_d.lib odbccp32.lib portaudio.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"$(DSAMDIR)\lib" /libpath:"$(PORTAUDIO)\pa_win\msvc"

!ENDIF 

# Begin Target

# Name "AMS_ng - Win32 Release"
# Name "AMS_ng - Win32 Debug"
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
# End Group
# End Target
# End Project
