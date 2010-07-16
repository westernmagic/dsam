%
% Calculate the amplitude in microPascals.
%
% Usage: mPa = DBSPL2MicroPascals(dBSPL)
%

function mPa = DBSPL2MicroPascals(dBSPL)

numLevels = size(dBSPL, 2);

sqrt_2 = sqrt(2.0);
for i = 1:numLevels
	mPa(i) = 20.0 * 10^(dBSPL(i) / 20.0) * sqrt_2;
end

end %function

