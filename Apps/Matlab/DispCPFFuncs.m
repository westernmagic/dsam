% This function Plots the click-evoked phase/frequency functions.
%
% usage: DispCPFFuncs(<file name>, <dataRef>)
%
% file name -   This is the data file name.
% dataRef -     This is the information added in brackets to the title.

function DispCPFFuncs(fileName, dataRef)

	[pFFunc, pFTitles] = ReadTitledData(fileName);
	title(['Click-Evoked Phase/Frequency Functions (' dataRef ')']);
	xlabel('Frequency (Hz)');
	ylabel('Relative-stimulus Phase (degrees)');
%	gset key right;
%	gset nologscale x;
%	gset nologscale y;
	PlotMultiLine(pFFunc, pFTitles, 0);
