function DRNLinputOutput

% This program is in process of being altered from an ELP fit to 
% a simple IO demonstration.

cd C:\allProgs\collaborators\Lowel\tutorials\inputOutput
simFile='DRNLinpuOutput.sim';
allIntensity =[];
fprintf('\n using .sim file %s\n\n', simFile);


%Data as given
CF=10000;
testFrequencyRatios=[.5; .8; 1; 1.1; 1.2; 1.3];
testFrequencies=CF*testFrequencyRatios;
nFreqs=length(testFrequencies);
legendString=strvcat(num2str(testFrequencies));

testLevels=   [10 20 30 40 50 60 70 80 90];
nLevels=length(testLevels);
plotColors=('rgbmcb');

aParams=[4.5e4; 4.5e2; 4.5]
aParams=[9e2]
for aParamNum=1:length(aParams)
	aParam=aParams(aParamNum);
	 COMPRSCALEA_x=1.4;
	 COMPRSCALEA_y=0.82;
	COMPRSCALEA_x=log10(aParam);
	COMPRSCALEA_y=0;
	fprintf('a value=%g\n',10^(COMPRSCALEA_x+COMPRSCALEA_y*log10(CF)))
	figure(aParamNum)
	clf,	axis([0 100 1e-6 1e-2])

	
	%SAMS section
	OutputLevels=zeros(nLevels ,nFreqs);
	
	for frequencyNum=1:nFreqs
		testFrequency=testFrequencies(frequencyNum);
		inputLevel=[];      outputLevel=[];                  
		outputVelocities=[];
		for maskerLevelNumber=1:nLevels
			level=testLevels(maskerLevelNumber);
			pars=[...
					' FREQUENCY.Stim_PureTone_2.0	 ' num2str(testFrequency)...
					' INTENSITY.Stim_PureTone_2.0 ' num2str(level)...
					' COMPRSCALEA_PARAMETER.BM_DRNL.7	  0:' num2str(COMPRSCALEA_x)...      
					' COMPRSCALEA_PARAMETER.BM_DRNL.7	  1:' num2str(COMPRSCALEA_y)...      
					' CENTRE_FREQ.BM_DRNL.7	  0:' num2str(CF)...      
				];
			
			SAMSresultsFile= runDSAM ('', 'results', simFile, pars,'dat');
			outRMS=getRmsFromFile(SAMSresultsFile);
			outputVelocity=20*10^(outRMS/20);%??
			
			outputLevel=[outputLevel; outRMS];
			outputVelocities=[outputVelocities; outputVelocity];
			inputLevel=[inputLevel; level];
		end %testLevel
		semilogy(inputLevel',outputVelocities,plotColors(frequencyNum)), pause(.1)
		hold on
		%outputLevels is rms in dB re 20m/s (!)
		outputLevels(:,frequencyNum)=outputLevel;
	end %frequency
	legend(legendString,2)
	xlabel('input (dB)')
	ylabel('output(dB)')
	axis([0 100 1e-6 1e-2])
	
end
