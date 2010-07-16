%
% This function calculates the rate-intensity curve of model.
%

function pData = PlotPSTH(simFile, mainPars, freq, leveldB, mode)

if (nargin < 5)
	mode = 'c';
end

sampleRate = 100000.0;
dt = 1.0 / sampleRate;
signalDuration = 0.06;

signal = PureTone(freq, leveldB, signalDuration, dt);

pars = [ ...
	mainPars ...
	' psth_oc on' ...
];

switch (mode)
case 'java'
	RunDSAMSimJ.SetDt(dt);
	data = RunDSAMSimJ.RunSimScript(simFile, pars, signal);
	t = DataXAxis2(RunDSAMSimJ.GetOutputTimeOffset(), RunDSAMSimJ.GetDt(), ...
	  RunDSAMSimJ.GetLength());
otherwise
	inInfo.dt = dt;
	[data info] = RunDSAMSim(simFile, pars, signal, inInfo);
	t = DataXAxis2(info.outputTimeOffset, info.dt, info.length);
end % switch

plot(t, data(1,:))

end

