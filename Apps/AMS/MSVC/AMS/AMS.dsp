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
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I "$(WXWIN)\include" /I "$(WXWIN)\lib\vc_dll\msw" /I "." /I "$(WXWIN)\samples" /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I "$(WXWIN)/contrib/include" /I "." /I "$(WXWIN)\samples" /I "$(WXWIN)\include" /I "$(DSAMDIR)\include" /I "$(DSAMDIR)" /I "$(WXWIN)/include" /I "$(WXWIN)\lib\vc_dll\msw" /I "$(PORTAUDIO)/pa_common" /D "WXUSINGDLL" /D "NOPCH" /D "_WINDOWS" /D "GRAPHICS_SUPPORT" /D "__WXMSW__" /D "USE_GUI" /D HAVE_CONFIG_H=1 /D "WIN32" /D "EXTENSIONS_SUPPORT" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "$(WXWIN)\include" /i "$(WXWIN)\lib\vc_dll\msw" /i "." /i "$(WXWIN)\samples" /d "__WXMSW__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i "$(WXWIN)\include" /i "$(WXWIN)\lib\vc_dll\msw" /i "." /i "$(WXWIN)\samples" /i "$(DSAMDIR)\src\GrLib" /i "$(DSAMDIR)/include" /d "__WXMSW__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw26_core.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswdll\thread.exe" /libpath:"$(WXWIN)\lib\vc_dll"
# ADD LINK32 DSAM_g.lib DSAM.lib DSAM_ext.lib wxmsw26_ogl.lib wxbase26_net.lib wxmsw26_html.lib wxmsw26_core.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib winmm.lib comctl32.lib rpcrt4.lib wsock32.lib oleacc.lib portaudio.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"$(DSAMDIR)\lib" /libpath:"$(PORTAUDIO)\pa_win\msvc"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(WXWIN)\contrib\include" /I "$(WXWIN)\include" /I "$(DSAMDIR)\include" /I "$(DSAMDIR)" /I "$(WXWIN)/include" /I "$(WXWIN)\lib\vc_dll\msw" /I "$(PORTAUDIO)/pa_common" /D "NDEBUG" /D "__WINDOWS__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "_WINDOWS" /D "GRAPHICS_SUPPORT" /D "__WXMSW__" /D "USE_GUI" /D HAVE_CONFIG_H=1 /D "WIN32" /D "EXTENSIONS_SUPPORT" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /i "$(DSAMDIR)\src\GrLib" /i "$(DSAMDIR)/include" /i "$(WXWIN)/include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 DSAM_d.lib DSAM_g_d.lib wxbase26.lib wxmsw26_ogl.lib wxmsw26_core.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib odbccp32.lib ogl.lib portaudio.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /pdbtype:sept /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"$(DSAMDIR)\lib" /libpath:"$(PORTAUDIO)\pa_win\msvc"

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
# End Group
# End Target
# End Project
