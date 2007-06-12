function ExternalIn1()
pars = 'diag off';

sampleRate = 100000.0;
dt = 1.0 / sampleRate;
time = dt:dt:0.1;
freq = 1000;
leveldB = 30;
amp = 20 * sqrt(2.0) * 10^(leveldB / 20.0);
signal = amp * sin(2.0 * pi * freq * time);

figure(1);
title('Stimulus');
ylabel('Amplitude (uPa)');
xlabel('Time (ms)');
plot(time * 1000.0, signal);

info.dt = dt;

[data info] = RunDSAMSim('LGuineaPigCN001_721Hz.spf', pars, signal, info);

t = DataXAxis(info);
figure(2);
title('CN Model');
ylabel('Cell Membrane Potential (mV)');
xlabel('Time (ms)');
plot(t * 1000.0, data);
