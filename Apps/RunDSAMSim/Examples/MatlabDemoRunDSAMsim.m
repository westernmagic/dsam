
function matlabDemoRunDSAMsim
% matlabDemo is a skeleton script for running a modle using AMS_ng

% Set the directory to the location of the .sim file
% If the file is not in the current directory, specify the full file path
simFilePath= 'gammatoneDemo.sim';

% Specify any parameters to be changed.
% This is a single continuous string of pairs of parameters:
%    <name> <value> <name> <value>    etc.
% To find the correct form of each parameter name, create a .spf file from
% the .sim file
lowestBF=1000;  % Hz
highestBF=1500; % Hz
info.numChannels= 1; % #

pars=[ ...
         ' MIN_CF.BM_gammaT.gammaT ' 	num2str(lowestBF) ...
         ' MAX_CF.BM_gammaT.gammaT '    num2str(highestBF) ...
         ' CHANNELS.BM_gammaT.gammaT '  num2str(info.numChannels) ...
     ];
 
% Specify diagnostic mode
diagMode='ON';
 
% Create the input signal 
frequency=1000;     % Hz
duration= 0.1;      % s
sampleRate=100000;  % Hz
dt=1/sampleRate;
t=dt:dt:duration;   % NB first time is dt (not 0).
signal= sin(2*pi*frequency*t);

plot(t, signal)

% store details for AMS
info.dt=dt;
info.length=length(signal)

% Now run AMS
[data info]=runDSAMsim (simFilePath, pars, 'ON', signal, info);

info
figure(2)
plot(t, data)


