function Bern(ExperimentType)
%Bern computes FDL for diotic and dichotic harmonic sequences

cd C:\allProgs\collaborators\Oxenham\binauralPitch
figure(2); clf
figure(1); clf

baseF0=100; numHarmonics=12;
duration=.1; dt=0.00001;

maxLag=0.012; lags=0:dt:maxLag;

simFile='pitchEarlyCrossover.sim';
%or
simFile='pitchLateCrossover.sim';
simFile

threshold=3e-7;
resultsSummary=[];
for firstHarmonic=3:3:25
   % first reference stimulus
   signalfile ='dichoticHarms.wav';
   SAMSgain=dichoticHarmonicStimulus(signalfile, baseF0,numHarmonics, firstHarmonic, duration, dt);
   responseToReferenceStimulus=computeSAMS(simFile,signalfile, maxLag,SAMSgain);
   t=responseToReferenceStimulus(1,:)';
   responseToReferenceStimulus=responseToReferenceStimulus(2,:)';
   
   EuclidDistance=0; previousEuclidDistance=0;      
   F0=baseF0;
   while EuclidDistance<threshold
      F0=F0+.25;
      SAMSgain=dichoticHarmonicStimulus(signalfile, F0,numHarmonics, firstHarmonic, duration, dt);
      responseToTargetStimulus=computeSAMS(simFile,signalfile, maxLag,SAMSgain);
      responseToTargetStimulus=responseToTargetStimulus(2,:)';
      
      figure(1), clf
      plot(t, responseToReferenceStimulus), hold on
      plot(t, responseToTargetStimulus,'r')
      axis([0 0.012 0 2e-4])
      title( num2str(firstHarmonic))
      
      EuclidDistance=sum((responseToReferenceStimulus-responseToTargetStimulus).^2);
      if previousEuclidDistance>EuclidDistance
         error('nonmonotonic euclidean distance')
      end
      previousEuclidDistance=EuclidDistance
      fprintf('%6.0f%8.2f   %g\n', firstHarmonic,F0, EuclidDistance);
      pause(1)
   end
   resultsSummary=[resultsSummary;[firstHarmonic F0]];
   fprintf('\n');
   figure(2); plot(resultsSummary(:,1), resultsSummary(:,2))
   hold on
   
end %harmonic sequence
