function Simple1()

pars = 'server_mode off diag screen duration 0.03';
[data, info] = RunDSAMSim('LGuineaPigCN001_721Hz_HS1.spf', pars);
t = DataXAxis(info);

figure(1);
title('CN Model');
ylabel('Cell Membrane Potential (mV)');
xlabel('Time (ms)');
plot(t * 1000.0, data);
plot(t, data);
