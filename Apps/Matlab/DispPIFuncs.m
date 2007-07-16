% This function Plots the phase/intensity functions.
%
% usage: DispPIFuncs(<file name>, <dataRef>)
%
% file name -     This is the data file name.
% dataRef -       This is the information added in brackets to the title.

function DispPIFuncs(fileName, dataRef)

	[pIFunc, pITitles] = ReadTitledData(fileName);
	title(['Phase/Intensity Functions (' dataRef ')']);
	xlabel('Input intensity (dB SPL)');
	ylabel('Relative Phase (cycles)');
	%gset key outside;
	%gset nologscale x;
	%gset nologscale y;
	PlotMultiLine(pIFunc, pITitles, -1);

