function outputname = runSpeechDemoAMS(inputfile, outputpath, simfile, pars, opformat, exe_path, exe_name)
% runAMS    run a DSAM application (default is AMS_NG) 
% This is a straight copy of runDSAM from Chris Sumner's dsam_mat folder
%
% SYNOPSIS:
% Function to run a DSAM application from MATLAB. The default is
% AMS_NG. It runs  any specified 'sim' file. 
%
% USE:
% runAMS(inputfile, outputpath, simfile [, pars, opformat ,
%   exe_path, exe_name] )
%   inputfile:  input file name ('' means no input file name change). 
%   outputpath: where to write the output dfile.
%   simfile:    simulation file defining the process modules
%   pars:       parameters for the simulation modules, overriding 
%               par file settings (optional).
%   opformat:   data format for the output file. 'dat' is DSAM text
%               format; other formats are supported by DSAM, but only
%               'aiff' will preserve magnitudes (optional).
%               'parameters' will not run the module but list the 
%               parameters used by the simulation (optional).
%   exe_name:   name to override the default DSAM executable being run.
%               (optional)
%   exe_path:   path to override the default DSAM executable being run.
%               (optional)
%               N.B. the default is AMS_NG.
%
% PARAMETERS (pars):
%   Parameters are specified in a string, separated by spaces.
%   the format for each is:
%         <var_name> <var_value>
%   For information on parameters available, call 
%       runAMS(<inputfile>,'',<simfile>,'pars')
%   This lists the processes and the parameters for the sim spec. 
%   It does not run the simulation at the same time.
%
% COMMENTS:
% The default output format is currently 'aiff' i.e. DSAM AIFF format.
% This includes automatic normalisation and so is recommended.
%
% In order to run AMS, the path for it must be set properly. You can do 
% using setAMSPath. Under Windows/NT etc., AMS must be installed in a 
% path without any white space.
%
% Applications are run with segment mode turned off.

% ------ Path for the aim routine -------
if nargin<7
   load aimMatPaths;
   amsdsam_path  = ams_path;
   amsdsam_exe = ams_exe;
else
   amsdsam_path = exe_path;
   amsdsam_exe = exe_name;
end;
   
% ------ parameter names for input and output -----
ipfilepar = 'FILENAME.DataFile_In';
opfilepar = 'FILENAME.DataFile_Out';
opapp = 'DSAM';		                     % Output appendment
dfltopformat = 'aiff';	                     % default output file type

% ------ Check sim file is specified ------
if nargin < 3
   error('runAMS: No sim file specified.');
end;

% ------ Check for parameter options -------
if nargin < 4
  pars='';
end;

% ------ Check file ouput file type option -----
if nargin<5
   opend = dfltopformat; 
else
   opend = opformat;
end;

% -------- Check for output path -------------
if nargin<3
  outputpath = '';
end;


% ----- Check input file, and generate output file name -------
if ~isempty(inputfile)

   % ----- Separate out the input filename ----------
   [pathname,filename,ending] = str2FileName(inputfile);

   % ------ Check the input file exists -------------
   if ~isAFile(inputfile)
      error('runSimPM: Input file does not exist.');
   end;

   % ------ Remove the input file run number -------- 
   shortfilename = rmParInStr('_', filename);
   
else 
   shortfilename  = 'op';
end;
   
% ------ Get the name of the output file ---------
stra = [outputpath shortfilename opapp];
strb = ['.' opend];
outputfile = genFileName(stra,strb);
[p outputname] = str2FileName(outputfile);

% Look for requests for lsting of the parameters, and remove them
% from the pars string.
parameterlistcmd = '';
pars_inds = findstr(pars,'pars');
if ~isempty(pars_inds) 
  parameterlistcmd = ' -l parameters ';
  pars = [pars(1:pars_inds(1)-1) pars( pars_inds(1)+4: length(pars) )]
end;
parameters_inds = findstr(pars,'parameters');
if ~isempty(parameters_inds) 
  parameterlistcmd = ' -l parameters ';
  pars = [pars( 1:parameters_inds(1)-1 ) ...
        pars( parameters_inds(1)+10:length(pars) )]
end;

% -------- prepare command line options list ------------
% Has no effect since input and output changes are not performed 
% at the mo'
if ~isempty(inputfile) 
   inputpar = [ipfilepar ' ' inputfile];
else 
   inputpar = '';
end;
 
outputpar = [opfilepar ' ' outputfile];
allpars = [pars ' ' inputpar ' ' outputpar];

% -------- Run the DSAM program (usually AMS) -------------------
%fprintf('\n----------------------------------------------------------\n');
%fprintf('genSim:\t\t1. Running simu....\n\n');
%eval(slash4OS(['!' amsdsam_path amsdsam_exe ' ' simfile ' ' allpars]));
cmd = ['!' amsdsam_path amsdsam_exe parameterlistcmd ' -s ' simfile ' -d off SEGMENT_MODE OFF ' allpars ];
eval(slash4OS(cmd));
%fprintf('----------------------------------------------------------\n');








 