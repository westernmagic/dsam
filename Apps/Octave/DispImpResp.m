# This function plots the impulse responses.
#
# usage: DispImpResp(<file name>, <separation?, <dataRef>)
#
# platform -      This is the platform used, 'octave' or 'matlab'
# file name -     This is the data file name.
# separation -    The y separation between each function.
# dataRef -       This is the information added in brackets to the title.

function DispImpResp(fileName, separation, dataRef)

	[funcs, titles] = ReadTitledData(fileName);
	title(['Impulse Responses (' dataRef ')']);
	xlabel('Time (s)');
	ylabel('BM Velocity (m/s)');
	gset key right;
	gset noytics;
	PlotMultiGraph(funcs, titles, separation);
	
	gset ytics;

