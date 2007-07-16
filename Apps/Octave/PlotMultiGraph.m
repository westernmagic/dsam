# This function plots the multiple graph lines.
#
# usage: PlotMultiGraph(<functions>, <titles>, <separation>)
#
# functions -   Function matrix.
# titles -      Function titles.
# speration -   Separation between functions.

function PlotMultiGraph(functions, titles, separation)

	hold on
	cols = size(functions, 2);
	func = functions;
	for i = 2:cols
		yOffset = (i - 2) * separation;
		func(:, i) = func(:, i) + yOffset;
	end
	for i = 2:cols
		plotTitle = deblank(titles(i, :));
		plot(func(:, 1), func(:, i), sprintf(';%s;', plotTitle));
	end
	hold off
