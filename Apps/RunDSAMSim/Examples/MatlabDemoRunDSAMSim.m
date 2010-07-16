
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


if (strcmp(mode, 'java') == 1)
	MyJavaAddClassPaths({'/home/lowel/Work/ExpData/Matlab/browser2/java', ...
	  '/home/lowel/Work/workspace/RunDSAMSimJ/bin'});
end

tic();
for fig = figRange
	figure(fig)
	switch (fig)
	case 1
		PlotPSTH(simFilePath, mainPars, bF, 80, mode);
	case 2
		PlotRateIntensity('GPDRNL57_AN057_OC039.spf', mainPars, bF, 0, 90, 10, mode)
	otherwise
		error(['Unknown figure requested: ' num2str(fig)]);
	end % switch
end
TimeReport(toc());

end % function

