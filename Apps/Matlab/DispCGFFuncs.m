% This function plots the click gain/frequency response functions.
%
% usage: DispCGFFuncs(<file name>, <dataRef>)
%
% file name -   This is the data file name.
% dataRef -     This is the information added in brackets to the title.

function DispCGFFuncs(fileName, dataRef)

	[cGFFuncs, cGFFTitles] = ReadTitledData(fileName);
	title(['Click-Evoked Gain/Frequency functions (' dataRef ')']);
	xlabel('Frequency (Hz)');
	ylabel('Gaim (m/s) / uPa');
%	gset logscale y
%	gset key outside;
%	gset key top;
%	gset nologscale x
	set(gca, 'YScale', 'log');
	SemiLogYMultiLine(cGFFuncs, cGFFTitles, -1);
%	gset nologscale y
