% This function Plots the phase/frequency functions.
%
% usage: DispPFFuncs(<file name>, <dataRef>)
%
% file name -   This is the data file name.
% dataRef -     This is the information added in brackets to the title.

function DispPFFuncs(fileName, dataRef)

	[pFFunc, pFTitles] = ReadTitledData(fileName);
	title(['Phase/Frequency Functions (' dataRef ')']);
	xlabel('Frequency (Hz)');
	ylabel('Relative Phase (radians)');
	%gset key right;
	%gset nologscale x;
	%gset nologscale y;
	PlotMultiLine(pFFunc, pFTitles, 1);
