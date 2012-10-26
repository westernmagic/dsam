
function MatlabDemoRunDSAMSim(figRange, mode)
if (nargin < 2)
	mode = 'c';
end

% matlabDemo is an example script for running a model using RunDSAMSim

% Set the directory to the location of the .spf file
% If the file is not in the current directory, specify the full file path
simFilePath = 'GPDRNL57_AN057_OC039.spf';

% Specify any parameters to be changed.
% This is a single continuous string of pairs of parameters:
%    <name> <value> <name> <value>    etc.
% To find the correct form of each parameter name, create a .spf file from
% the .sim file

mainPars = [ ...
	' diag_mode off' ...
];

bF = 8018.0;

tic();
for fig = figRange
	figure(fig)
	switch (fig)
	case 1
		PlotPSTH(simFilePath, mainPars, bF, 80, mode);
		title('VCN Onset-C Cell PSTH'); 
	case 2
		PlotRateIntensity(simFilePath, mainPars, bF, 0, 90, 10, mode)
		title('VCN Onset-C Cell Rate-Intensity Function'); 
	otherwise
		error(['Unknown figure requested: ' num2str(fig)]);
	end % switch
end
TimeReport(toc());

end % function

