% This function prints the figure.
%
% usage PrintFigure(<int>)
%
%  - where int is zero for output to the screen

function PrintFigure(print, fileName)

	if (print ~= 0)
		switch print
			case 4
%				gset term gif;
			disp('Need to set for giff output');
			otherwise
%				gset term postscript color;
				SetPrintOutput(print);
		end
	end

