%
% This function returns a report of elapsed time using the appropriate units.
%

function TimeReport(elapsedSeconds)

seconds = mod(elapsedSeconds, 60);
minutes = rem((elapsedSeconds - seconds)/60, 60);
hours = fix(elapsedSeconds / 3600);

str = sprintf('Elapsed time = %g seconds ', elapsedSeconds);
if (hours || minutes)
	str = [str sprintf('(')];
	if (hours)
		str = [str sprintf('%d hour', hours)];
		if (hours > 1)
			str = [str sprintf('s')];
		end
	end
	if (minutes)
		if (hours)
			if (seconds)
				str = [str sprintf(', ')];
			else
				str = [str sprintf(' and ')];
			end
		end
		str = [str sprintf('%d minute', minutes)];
		if (minutes > 1)
			str = [str sprintf('s')];
		end
	end
	if (seconds)
		str = [str sprintf(' and %d second', seconds)];
		if (seconds > 1)
			str = [str sprintf('s')];
		end
	end
	str = [str sprintf(')')];
end
disp(str);
end % function


