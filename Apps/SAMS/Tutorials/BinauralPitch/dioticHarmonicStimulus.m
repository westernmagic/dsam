function SAMSgain=dioticHarmonicStimulus(fileName, F0,numHarmonics, firstHarmonic, duration, dt)
% cd C:\allProgs\collaborators\Oxenham\binauralPitch

% 0dB SPL is signal level per component in micropascals

t=dt:dt:duration; t=t';

leftStimulus=zeros(length(t),1);
for harmNum=firstHarmonic:1:firstHarmonic+numHarmonics
   leftStimulus=leftStimulus+20*sin(2*pi*F0*harmNum*t);
end

rightStimulus=zeros(length(t),1);
for harmNum=firstHarmonic:1:firstHarmonic+numHarmonics
   rightStimulus=rightStimulus+20*sin(2*pi*F0*harmNum*t);
end
%plot(t,signal);

signal=leftStimulus+rightStimulus;
signal=[signal signal];


normFactor=max(max(abs(signal)));
signal=signal/(1.01*normFactor);
wavwrite(signal,1/dt,16, fileName)

SAMSgain=20*log10(normFactor);
figure(1)
plot(signal(:,1))
figure(2)
plot(signal(:,2))