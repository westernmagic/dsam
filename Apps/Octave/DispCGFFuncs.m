# This function plots the click gain/frequency response functions.
#
# usage: DispCGFFuncs(<file name>, <dataRef>, [<lTitles>])
#
# platform  -   This is the platform used, 'octave' or 'matlab'
# file name -   This is the data file name.
# dataRef   -   This is the information added in brackets to the title.
# lTitles   -   Legend titles if required.

function DispCGFFuncs(fileName, dataRef, lTitles)

	if nargin > 2
		[cGFFuncs] = ReadTitledData(fileName);
	else
		[cGFFuncs lTitles] = ReadTitledData(fileName);
	end
	title(['Click-Evoked Gain/Frequency functions (' dataRef ')']);
	xlabel('Frequency (Hz)');
	ylabel('Gaim (m/s) / uPa');
#	gset logscale y
	gset key outside;
	gset key top;
#	gset nologscale x
	SemiLogYMultiLine(cGFFuncs, lTitles);
#	gset nologscale y
