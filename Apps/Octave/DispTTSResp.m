# This function plots the 2-tone suppression responses.
#
# usage: DispTTSResp(<file name>, <dataRef>)
#
# platform -      This is the platform used, 'octave' or 'matlab'
# file name -     This is the data file name.
# dataRef -       This is the information added in brackets to the title.

function DispTTSResp(fileName, dataRef)

	[funcs, titles] = ReadTitledData(fileName);
	title(['2-Tone Suppression Responses (' dataRef ')']);
	xlabel('Time (s)');
	ylabel('BM Velocity (m/s)');
	gset key right;
	PlotMultiLine(funcs, titles);
