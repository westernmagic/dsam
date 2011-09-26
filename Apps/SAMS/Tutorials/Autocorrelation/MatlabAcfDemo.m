
function MatlabAcfDemo (simFilePath, simFileName, outputFileName)
% matlabDemo is a skeleton script for running a modle using SAMS_ng

% Set the directory to the location of the .sim file
if ~isempty(simFilePath)
    cd (simFilePath);
end

%default output file name is assumed to be output.dat
if nargin<3
    outputFileName='output.dat';
end

% Prepare for plotting later
figure(1), clf, plotColors='rgb'; colorCount=0;

% Control the lowest and highest harmonic numbers
F0=200; % Hz
numberOfHarmonics=10;
for lowestHarmonic=3:3:9
    highestHarmonic=lowestHarmonic+numberOfHarmonics;
    
    % Specify arguments for the runDSAM command
    pars=[  ...
            ' LOW_HARMONIC.Stim_Harmonic.stim ' num2str(lowestHarmonic) ...
            ' HIGH_HARMONIC.Stim_Harmonic.stim  ' num2str(highestHarmonic) ...
            ' FUND_FREQ.Stim_Harmonic.stim ' num2str(F0) ...
        ];
    
    
    % Now run SAMS
    runSAMS_RM (simFileName,pars);
    
    % When the program reaches here, SAMS should have created a results file
    [results, column1Values, columnHeadingsValues, errorMsg]=...
        readDatFile(outputFileName);    
    if errorMsg==1
        disp('error when reading output file')
        return
    end
    
       % The first row is time, the second is the SACF
   time=column1Values;
   sacf=results; 
   
   % Plot it
   colorCount=colorCount+1;
   plot(time,sacf, plotColors(colorCount)), hold on
   
end
legend ('3','6','9',2)
xlabel ('autocorrelation lag (s)')


    
    
    
    %--------------------------------------------------------------------------
    % --------------------------------------------------------read data file
    function [results, column1Values, columnHeadingsValues, errorMsg]=readDatFile(datFileName)
    % readDatFile get results from file created by SAMS
    % It is assumed that the .dat file consists of
    % a first row with short text followed by heading values
    % later rows are row heading followed by results
    
    errorMsg=0;
    try
        fid = fopen(datFileName);
    catch
        errorMsg=1; return
    end   
    
    columnHeadings = fgetl(fid); 	%column headings     
    
    % assume that the first 9 characters are text
    closingParenthesis=1+ findstr(columnHeadings, ')');
    columnHeadings=columnHeadings(closingParenthesis:end); %remove 'Time (s)'
    
    % extract BFs
    columnHeadingsValues=str2num(columnHeadings); 
    numHeadings=length(columnHeadingsValues);
    
    x= fscanf(fid,'%g',[numHeadings+1 inf]);
    x=x';
    
    column1Values=x(:,1);
    results=x(:,2:end);
    
    fclose(fid); 
    
    function tempFileName=setNoRepeats (simFile, noRepeats)
    % setNoRepeats alters .sim file to set repeats as required
    % tempFileName=setNoRepeats (simFile, noRepeats)
    
    tempFileName='tempSimFile.sim';
    fidIn = fopen(simFile);
    if fidIn == -1, error('simFile not found'), end
    fidOut = fopen(tempFileName,'w');
    if fidOut == -1, error('setNoRepeats, tempSimFile.sim could not be opened'), end
    
    while 1
        line = fgetl(fidIn);   if ~ischar(line), break, end
        if ~isempty(findstr(line, 'repeat'));
            if ~isempty(findstr(line, '{'));
                line=['repeat ' num2str(noRepeats) ' {'];
            end
        end
        
        %  disp(line)
        fprintf(fidOut,'%s\n', line);  
    end
    fclose(fidOut);   fclose(fidIn);
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    
    % --------------------------------------------- runSAMS
    function runSAMS_RM(simFile, pars)
    % runSAMS*    run SAMS application using SAMS_ng.exe 
    % This is a simplified copy of runDSAM from Chris Sumner's dsam_mat folder
    %
    % SYNOPSIS:
    % Function: to run the SAMS application from MATLAB. 
    % It runs  any specified 'sim' file. 
    % This is a simplified version that assumes 
    % 1. You are using sams_ng as the executable.
    % 2. The executable is to be found in 'c:\Progra~1\DSAM\SAMS\'
    %
    % USE:
    % runSAMS(simFile, pars)
    %   simFile:    simulation file defining the process modules
    %   pars:       stimuli for the simulation modules, overriding 
    %               par file settings (optional).
    %
    % stimuli (pars):
    %   stimuli are specified in a string, separated by spaces.
    %   the format for each is:
    %        [ <variable name><space><variable value><space> ..]
    %
    %This function calls SAMS_ng.exe, which must be located on thepath:
    %  C:\Program Files\DSAM\SAMS
    %If you have located this executable somewhere else, you must change
    % the path name appropriately below.
    %
    % SEGMENT MODE is set to OFF
    
    % ------ Path for the aim routine -------
    samsdsam_path  = 'c:\Progra~1\DSAM\SAMS\';
    samsdsam_exe = 'sams_ng';
    
    % ------ Check sim file is specified ------
    if nargin < 1
        error('runSAMS: No sim file specified.');
    end;
    
    % ------ Check for stimulus options -------
    if nargin < 2
        pars='';
    end;
    
    % Look for requests for lsting of the stimuli, and remove them
    % from the pars string.
    stimuluslistcmd = '';
    pars_inds = findstr(pars,'pars');
    if ~isempty(pars_inds) 
        stimuluslistcmd = ' -l stimuli ';
        pars = [pars(1:pars_inds(1)-1) pars( pars_inds(1)+4: length(pars) )]
    end;
    stimuli_inds = findstr(pars,'stimuli');
    if ~isempty(stimuli_inds) 
        stimuluslistcmd = ' -l stimuli ';
        pars = [pars( 1:stimuli_inds(1)-1 ) ...
                pars( stimuli_inds(1)+10:length(pars) )]
    end;
    
    % -------- Run the DSAM program (usually SAMS) -------------------
    cmd = ['!' samsdsam_path samsdsam_exe stimuluslistcmd ' -s ' simFile ' -d off SEGMENT_MODE OFF ' pars ];
    eval(slash4OS(cmd));
    
    %-----------------------------------------------------------------------------------
    function strout = slash4OS(strin)
    % strout = slash4OS(strin)
    % Takes a string, containing paths, and converts any slashes to the correct
    % form for the underlying operating system, automatically determining OS
    
    if (computer=='PCWIN')
        newslash = '\';
        oldslash = '/';
    else 
        newslash = '/';
        oldslash = '\';
    end;
    
    oldslashpos = findstr(strin,oldslash);
    strin(oldslashpos) = ones(size(oldslashpos))*newslash;
    strout = strin;
