function DRNL_RIfunction 
% DRNL_RIfunction computes the rate/intensity function for a DRNL filter
% 'Intensity' is the peak amplitude (m/s) in the last half of the signal
% pureToneResponse.sim
% DRNL_TestALN.par

CFlist=[500 1000 2000 4000 8000];
CFnum=0; numCFs=length(CFlist);
figure(1), clf, figure(2), clf
for CF=CFlist
   CFnum=CFnum+1;
cd C:\allProgs\collaborators\Lowel\tutorials\impulseResponse
simFile='pureToneResponseELP.sim';

allIntensity =[];  frequencyList=[];

duration=0.05; 
fprintf('CF= %6.0f\n', CF)

frequencyRatios=[.1 .3 .5 .7 .9  1 1.1 1.3 1.5 1.7 1.9 2];
frequencyList=CF*frequencyRatios;
for frequency=frequencyList
   fprintf('frequency= %6.0f\n',frequency)
   outputIntensity=[];
   inputIntensity=[0:10:100];
   inputIntensity=[0:20:100];
   for intensity= inputIntensity
      pars=[...
            ' INTENSITY.Stim_PureTone.stim	' num2str(intensity)...
            ' FREQUENCY.Stim_PureTone.stim '	num2str(frequency)...
            ' DURATION.stim_PureTone.stim	' num2str(duration) ... 
            ' SINGLE_CF.BM_DRNL.BM '      	num2str(CF) ...
         ];
      
      AMSresultsFile= runDSAM ('', 'results', simFile, pars,'dat');
      
      fid = fopen(AMSresultsFile);
      if fid == -1, error('AMS did not produce a file'),   end  
      
      ignorFirstLine = fgetl(fid); 	%column headings (ignore)
      
      %read two column matrix (time, 0/1 spike event)
      x= fscanf(fid,'%g %g',[2 inf]);
      fprintf('intensity in/out= %10.0f\t%g\n',intensity, x(2,1))
      outputIntensity=[outputIntensity; x(2,1)];
      fclose(fid); delete results*
   end
   
   allIntensity=[allIntensity outputIntensity];
end
figure(1)
subplot(1,numCFs,CFnum)
semilogy(inputIntensity, allIntensity), hold on
xlabel('input dB SPL')
% ylabel('output m/s')
ylim([1e-6 1e-3])
% legend(strvcat(num2str(frequencyList')),2)
title([ 'CF= ' num2str(CF)])

figure(2)
subplot(1, numCFs,CFnum)
loglog(frequencyList, allIntensity'), hold on
xlabel('frequency')
% ylabel('output m/s') 
ylim([1e-6 1e-3])
% legend(strvcat(num2str(inputIntensity')),1)
title([ 'CF= ' num2str(CF)])
pause(.1)
end