% This function plots the input output functions.
%
% usage: DispIOFuncs(<file name>, <dataRef>, <printVelocity>)
%
% file name -     This is the data file name.
% dataRef -       This is the information added in brackets to the title.
% printVelocity - Sets output to velocity (1) or intensity (0) form.

function DispIOFuncs(fileName, dataRef, printVelocity)

	[iOFunc, iOTitles] = ReadTitledData(fileName);
	title(['Input/Output Functions (' dataRef ')']);
	xlabel('Input intensity (dB SPL)');
	if (printVelocity ~= 0)
		ylabel('Peak Velocity (m/s)');
		SemiLogYMultiLine(iOFunc, iOTitles, -1);
	else
		ylabel('Output Intensity (dB SPL)');
		PlotMultiLine(iOFunc, iOTitles, -1);
	end
%	gset key outside;
%	gset nologscale x
