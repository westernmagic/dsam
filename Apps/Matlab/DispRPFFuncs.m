% This function Plots the phase/frequency functions.
%
% usage: DispRPFFuncs(<file name>, <dataRef>)
%
% file name -   This is the data file name.
% dataRef -     This is the information added in brackets to the title.

function DispRPFFuncs(fileName, dataRef)

	[rPFFunc, rPFTitles] = ReadTitledData(fileName);
	title(['Relative Phase/Frequency Functions (' dataRef ')']);
	xlabel('Frequency (Hz)');
	ylabel('Relative Phase (radians)');
%	gset key outside;
%	gset nologscale x;
%	gset nologscale y;
	PlotMultiLine(rPFFunc, rPFTitles, -1);
