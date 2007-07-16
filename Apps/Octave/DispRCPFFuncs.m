# This function Plots the click-evoked phase/frequency functions.
#
# usage: DispRCPFFuncs(<file name>, <dataRef>)
#
# platform -    This is the platform used, 'octave' or 'matlab'
# file name -   This is the data file name.
# dataRef -     This is the information added in brackets to the title.

function DispRCPFFuncs(fileName, dataRef)

	[funcs, titles] = ReadTitledData(fileName);
	title(['Relative Click Phase/Frequency Functions (' dataRef ')']);
	xlabel('Frequency (Hz)');
	ylabel('Relative Phase (degrees)');
	gset key outside;
#	gset nologscale x;
#	gset nologscale y;
	PlotMultiLine(funcs, titles);
