% This function plots the Distortion Products Routine.
%
% usage: DispDProds(<file name>, <dataRef>)
%
% file name -   This is the data file name.
% dataRef -     This is the information added in brackets to the title.

function DispDProds(fileName, dataRef)

	[funcs, titles] = ReadTitledData(fileName);
	title(['Distortion Products (' dataRef ')']);
	xlabel('Frequency (Hz)');
	ylabel('Abitrary FFT Amplitude');
	set(gca, 'YScale', 'log');
	SemiLogYMultiLine(funcs, titles, 0);
