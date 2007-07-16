% This function Plots the tuning curve data.
%
% usage: DispTCurve(<file name>, <dataRef>)
%
% file name -   This is the data file name.
% dataRef -     This is the information added in brackets to the title.

function DispTCurve(fileName, dataRef);

	[tCurve, tCTitle] = ReadTitledData(fileName);
	semilogx(tCurve(:,1), tCurve(:,2));
	leghdl = legend(tCTitle(2, 1:5), 2);
	set(leghdl, 'visible', 'off');
	title(['Tuning curve (' dataRef ')']);
	xlabel('Frequency (Hz)');
   ylabel('Threshold');
