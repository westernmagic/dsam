% This function plots the input output functions.
%
% usage: DispTTSRatio(<file name>, <dataRef>)
%
% file name -     This is the data file name.
% dataRef -       This is the information added in brackets to the title.

function DispTTSRatio(fileName, dataRef)

	[tTSFunc, tTSTitles] = ReadTitledData(fileName);
	title(['2-Tone Suppression Ratio Functions (' dataRef ')']);
	xlabel('Suppresser intensity, T2 (dB SPL)');
	ylabel('Relative Intensity (dB)');
	%gset key left;
	%gset nologscale x;
	%gset nologscale y;
	PlotMultiLine(tTSFunc, tTSTitles, 0);
