%
% This function calculates the rate-intensity curve of model.
%

function pData = PlotRateIntensity(simFile, mainPars, freq, initialInt, finalInt, ...
  deltaInt, mode)

if (nargin < 7)
	mode = 'c';
end

steadyStateFactor = 0.9;
sampleRate = 100000.0;
dt = 1.0 / sampleRate;
signalDuration = 0.1;
time = dt:dt:signalDuration;

pars = [ ...
	mainPars ...
	' psth_oc on' ...
];

inInfo.dt = dt;
count = 0;
level = initialInt:deltaInt:finalInt;
for leveldB = level
	signal = PureTone(freq, leveldB, signalDuration, dt);
	switch (mode)
	case 'java'
		RunDSAMSimJ.SetDt(dt);
		data = RunDSAMSimJ.RunSimScript(simFile, pars, signal);
		info = GetRunDSAMSimJInfo();
	otherwise
		inInfo.dt = dt;
		[data info] = RunDSAMSim(simFile, pars, signal, inInfo);
		t = DataXAxis2(info.outputTimeOffset, info.dt, info.length);
	end % switch
	if (leveldB == initialInt)
		offsetSamples = info.length * steadyStateFactor;
		offsetTime = offsetSamples * info.dt;
		calcDuration = (info.length - offsetSamples) * info.dt;
	end
	count = count + 1;
	rate(count) = mean(data(1, offsetSamples:info.length)) / calcDuration;
end
plot(level, rate);

end % function


