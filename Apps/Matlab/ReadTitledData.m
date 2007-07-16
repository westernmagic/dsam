% This routine reads an xy data file.
%
% usage [var] = ReadTitledData(<fileName>)


function [data, titles] = ReadTitledData(fileName)

	disp(sprintf('Reading the file "%s"\n', fileName));
	[myFile, msg] = fopen(fileName, 'r');
	if (myFile == -1)
		disp(sprintf('Error: %s\n', msg));
		data = 0.0;
		titles = '';
		return
	end
	titleLine = fgetl(myFile);
	titles = split(titleLine, sprintf('\t'));
	rows = size(titles, 1);
	newData = fscanf(myFile, '%g %g', [rows, Inf]);
	data = newData';

