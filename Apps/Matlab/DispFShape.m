% This function plots the filter shape functions.
%
% usage: DispFShape(<file name>, <dataRef>, <printVelocity>)
%
% file name -     This is the data file name.
% dataRef -       This is the information added in brackets to the title.
% printVelocity - Sets output to velocity (1) or intensity (0) form.

function DispFShape(fileName, dataRef, printVelocity)

	[fShape, fSTitles] = ReadTitledData(fileName);
	title(['Filter shape Functions (' dataRef ')']);
	xlabel('Frequency (Hz)');
	if (printVelocity == 1)
		ylabel('Peak Output Velocity (m/s)');
		LogLogMultiLine(fShape, fSTitles, 3);
	else
		ylabel('Input Intensity (dB SPL)');
		SemiLogXMultiLine(fShape, fSTitles, 3);
	end
	%gset key right;
	%gset key top;
	%gset logscale x
