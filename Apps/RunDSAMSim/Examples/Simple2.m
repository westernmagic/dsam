function Simple2()

pars = 'server_mode off diag screen duration 10.0';
[data, info] = RunDSAMSim('test.spf', pars);
t = DataXAxis(info);

figure(1);
title('CN Model');
ylabel('Cell Membrane Potential (mV)');
xlabel('Time (ms)');
plot(t * 1000.0, data);
plot(t, data);
