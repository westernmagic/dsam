%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% File:			ReadWinFrame.m
% Purpose:		Reads a window from from an AIFF file.
% Comments:	
% Author:		L. P. O'Mard
% Revised by:
% Created:
% Updated:
% Copyright:	(c) 2000, University of Essex
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%

function frame=ReadWinFrame(fid, numChannels, frameLen, wordSize, littleEndian)

switch wordSize
	case 1
		frame = fread(fid, [numChannels, frameLen], 'char');
	case 2
		frame = Read16Bits(fid, littleEndian);
		if (littleEndian == 0)
			for i = 1:frameLen
				for j = 1:numChannels
					frame(j, i) = bitshift(bitand(frame(j, i), 255), +8) + ...
					  bitshift(frame(j, i), -8);
				end;
			end;
		end;
	case 4
		for i = 1:frameLen
			for j = 1:numChannels
				frame(j, i) = Read32Bits(fid, littleEndian);
			end;
		end;
	
end

