% This function plots the multiple graph lines.
%
% usage: SemiLogXMultiLine(<functions>, <titles>, <legend_pos>)
%
% functions -   Function matrix.
% titles -      Function titles.
% legend_pos -	Sets the legend position.

function SemiLogXMultiLine(functions, titles, legend_pos)

	hold on;
	cols = size(functions, 2);
	semilogx(functions(:, 1), functions(:, 2:cols));
	leghdl = legend(titles(2:cols, :), legend_pos);
	set(leghdl, 'visible', 'off');
	hold off;

