function MatlabAcfDemo


% Set the directory to the location of the .sim file
cd C:\allProgs\collaborators\Lowel\tutorials\autocorrelation

% Prepare for plotting later
figure(1), clf, plotColors='rgb'; colorCount=0;

% Control the lowest and highest harmonic numbers
F0=200; % Hz
numberOfHarmonics=10;
for lowestHarmonic=3:3:9
   highestHarmonic=lowestHarmonic+numberOfHarmonics;
   
   % Specify arguments for the runDSAM command
   signalFileName='';
   resultsFilenameIdentifier='results';
   simFile='ACF.sim';
   pars=[  ...
         ' LOW_HARMONIC.Stim_Harmonic.0 ' num2str(lowestHarmonic) ...
         ' HIGH_HARMONIC.Stim_Harmonic.0 ' num2str(highestHarmonic) ...
         ' FUND_FREQ.Stim_Harmonic.0 ' num2str(F0) ...
      ];
   resultsFilenameExtension='dat';
   
   % Now run AMS
   AMSresultsFile= runDSAM (signalFileName, resultsFilenameIdentifier, ...
      simFile,pars,resultsFilenameExtension);
   
   % When the program reaches here, 
   %  AMS should have created a results file
   fid = fopen(AMSresultsFile);
   if fid == -1, error('AMS did not produce a file'),   end   
   
   % The first line of the results file will typically 
   %  be a header line that should be ignored.
   ignoreFirstLine = fgetl(fid);  
   
   % The rest of the results should be in the form 
   %  of a two-column matrix as read in an editor,
   %  but MATLAB reads it as a two-row matrix
   results= fscanf(fid,'%g %g',[2 inf]);
   
   % The first row is time, the second is the SACF
   time=results(1,:);
   sacf=results(2,:); 
   
   % Plot it
   colorCount=colorCount+1;
   plot(time,sacf, plotColors(colorCount)), hold on
   
   % runDSAM creates a new file every time 
   %  and these will accumulate if not deleted.
   % Each file will contain the word 'results' 
   %  as specified in resultsFilenameIdentifier
   fclose(fid); 
   delete results*
end
legend ('3','6','9',2)
xlabel ('autocorrelation lag (s)')

