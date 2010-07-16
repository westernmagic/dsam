function [signal, time] = PureTone(frequency, leveldB, duration, dt)

if (nargin < 3)
	duration = 0.1;
end

if (nargin < 4)
	dt = 1.0 / 44100;
end

amp = DBSPL2MicroPascals(leveldB);
time = dt:dt:duration;
signal = amp * sin(2.0 * pi * frequency * time);

end % function

