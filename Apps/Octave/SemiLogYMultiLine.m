# This function plots the multiple graph lines.
#
# usage: SemiLogYMultiLine(<functions>, <titles>)
#
# functions -   Function matrix.
# titles -      Function titles.

function SemiLogYMultiLine(functions, titles)

	hold on;
	cols = size(functions, 2);
	for i = 2:cols
		if (size(titles, 1) > 1)
			plotTitle = deblank(titles(i, :));
		else
			plotTitle = titles;
		end
		switch floor((i - 2) / 6)
		  case 0
		  	semilogy(functions(:, 1), functions(:, i), ...
			  sprintf(';%s;', plotTitle));
		  case 1
			#gplot functions using 1:i title plotTitle with linespoints;
		  	semilogy(functions(:, 1), functions(:, i), ...
			  sprintf('-@;%s;', plotTitle));
		 otherwise
			#gplot functions using 1:i title plotTitle with points;
		  	semilogy(functions(:, 1), functions(:, i), ...
			  sprintf('@;%s;', plotTitle));
		end
	end
	hold off;

