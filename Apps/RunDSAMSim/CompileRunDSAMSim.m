%File:		DSAM_SDK.nsi
%Purpose:	Create the DSAM SDK required to compile DSAM programs
%Comments:	This script is based on NSIS example2.nsi
%		The "SetOutPath" is needed for the writing of the uninstaller and
%		and also the DLL install - even though the full directory is given
%		in the instruction line.
%Author:	L. P. O'Mard
%Created:	9 Mar 2009
%Updated:
%Copyright:	(C) 2009
%
% Usage: CompileRunDSAMSim
%

function CompileRunDSAMSim

mex -v RunDSAMSim.cpp MatMainApp.cpp ...
    COMPFLAGS="$COMPFLAGS -I$DSAMDIR\include\dsam -I$DSAMDIR\include\wxwin ...
        -I$DSAMDIR\include\portaudio -I$DSAMDIR\include\libsndfile ...
        -I$DSAMDIR\include\fftw -D__WINDOWS -D_CONSOLE -D__WXMSW__ ...
        -DWXUSINGDLL=1 -DwxUSE_GUI=0 -DWIN32 -DHAVE_CONFIG_H=1 -DEXTENSIONS_SUPPORT=1 ...
        -DLIBRARY_COMPILE=1 -DDSAM_USE_UNICODE=1 ...
        -D_WINDLL -D_UNICODE -DUNICODE" ...
    LINKFLAGS="$LINKFLAGS ...
        /LIBPATH:$DSAMDIR\lib\x86 ...
        dsam_x86.lib dsam_ext_x86.lib dsam_std_x86.lib ...
        wxbase28u_net.lib wxbase28u.lib wxbase28u_xml.lib ...
        portaudio.lib libsndfile_x86.lib libfftw.lib ...
        /NODEFAULTLIB:libCMT"

disp('Once the "RunDSAMSim.mexw??" has been successfully compiled');
disp('copy it and the "RunDSAMSim.m" file to the DSAM SDK DLL directory.');
disp('For an installation of the DSAM SDK using the default directories,');
disp('this will be:');
disp('');
disp('    c:\Program Files\Common Files\dsam');
disp('');
disp('on a 32-bit installation.');
disp('Once you have set this ensure that the DSAM DLL directory is');
disp('then included in the matlab path.');
 
end %function
