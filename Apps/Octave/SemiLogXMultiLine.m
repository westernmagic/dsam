# This function plots the multiple graph lines.
#
# usage: SemiLogXMultiLine(<functions>, <titles>)
#
# functions -   Function matrix.
# titles -      Function titles.

function SemiLogXMultiLine(functions, titles)

	hold on;
	cols = size(functions, 2);
	for i = 2:cols
		plotTitle = deblank(titles(i, :));
			switch floor((i - 2) / 6)
			  case 0
		  		semilogx(functions(:, 1), functions(:, i), ...
				  sprintf(';%s;', plotTitle));
			  case 1
				#gplot functions using 1:i title plotTitle with linespoints;
		  		semilogx(functions(:, 1), functions(:, i), ...
				  sprintf('-@;%s;', plotTitle));
			 otherwise
				#gplot functions using 1:i title plotTitle with points;
		  		semilogx(functions(:, 1), functions(:, i), ...
				  sprintf('@;%s;', plotTitle));
			end
	end
	hold off;

