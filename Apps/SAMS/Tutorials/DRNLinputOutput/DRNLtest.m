function DRNLtest
 
cd C:\allProgs\projects\impulseResponse

simFile='Alberto.sim';
allIntensity =[];
fprintf('\n using .sim file %s\n\n', simFile);
for frequency=4000:1000:10000
   frequency
outputIntensity=[];
inputIntensity=[];
for intensity= 0:10:100
   
   pars=[...
         ' INTENSITY.Stim_PureTone.0	' num2str(intensity)...
         '	FREQUENCY.Stim_PureTone.0 '	num2str(frequency)...
      ];
   
   SAMSresultsFile= runDSAM ('', 'results', simFile, pars,'dat');
   

   fid = fopen(SAMSresultsFile);
   if fid == -1, error('SAMS did not produce a file'),   end  
   
   ignorFirstLine = fgetl(fid); 	%column headings (ignore)
   
   %read two column matrix (time, 0/1 spike event)
   x= fscanf(fid,'%g %g',[2 inf]);
   fprintf('intensity= %10.0f\n',x(2,1))
   inputIntensity=[inputIntensity; intensity];
   outputIntensity=[outputIntensity; x(2,1)];
   fclose(fid); delete results*
end
allIntensity=[allIntensity outputIntensity];
end
plotHandle=plot(inputIntensity, allIntensity )
xlabel('input dB')
ylabel('output dB')
legend( plotHandle, '4000', '5000','6000','7000','8000','9000','10000',-1)

hgsave(plotHandle,'4to10kHz')