%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% File:			ReadAIFF.m
% Purpose:		Reads an AIFF format file.
% Comments:		The default binary format is big-endian, if this does not
%				work, then little-endian format is tried.
% Author:		L. P. O'Mard
% Revised by:
% Created:
% Updated:
% Copyright:	(c) 2000, University of Essex
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [signal, info] = ReadAIFF(fname)

absoluteNormalise = 1.0;
fid = fopen(fname);
littleEndian = 0;
aiff_form = ReadBytes(fid, 4, littleEndian);
if aiff_form' == double('FORM')
%	disp('big-endian');
else
%	disp('little-endian');
	littleEndian = 1;
end;

chunkSize = Read32Bits(fid, littleEndian);

aiff_aiff = ReadBytes(fid, 4, littleEndian);
if aiff_aiff' ~= double('AIFF')
	aiff_aiff
	disp('Not a valid AIFF file.');
	return
end;
chunkSize = chunkSize - 16;

while chunkSize - 4 > 0;
	chunkName = ReadBytes(fid, 4, littleEndian);
	chunkSize = chunkSize - 4;
	if chunkName' == double('COMM');
 		%disp('step COMM');
 		subSize = Read32Bits(fid, littleEndian);
 		chunkSize = chunkSize-subSize;
 		info.numChannels = Read16Bits(fid, littleEndian);
 		subSize = subSize - 2;
 		numSampleFrames = Read32Bits(fid, littleEndian);
 		subSize = subSize - 4; 
 		sampleSize = Read16Bits(fid, littleEndian);
 		subSize = subSize - 2;
 		sampleRate = ReadIEEE(fid, littleEndian);
 		subSize = subSize - 10;
 		fread(fid, subSize, 'char');
	elseif chunkName' == double('SSND');
 		%disp('step SSND');
 		subSize = Read32Bits(fid, littleEndian);
 		chunkSize = chunkSize - subSize;
 		offset = Read32Bits(fid, littleEndian);
		subSize = subSize - 4;
		blockSize = Read32Bits(fid, littleEndian);
		subSize = subSize - 4;
		soundPosition = ftell(fid) + offset;
		fread(fid, subSize, 'char');
	elseif chunkName' == double('LUT2');
 		%disp('step LUT2');
 		subSize = Read32Bits(fid, littleEndian);
 		chunkSize = chunkSize - subSize;
 		info.interleaveLevel = Read16Bits(fid, littleEndian);
 		subSize = subSize - 2;
  		info.numWindowFrames = Read16Bits(fid, littleEndian);
 		subSize = subSize - 2;
  		staticTimeFlag = Read16Bits(fid, littleEndian);
 		subSize = subSize - 2;
		info.outputTimeOffset = ReadIEEE(fid, littleEndian);
 		subSize = subSize - 10;
 		absoluteNormalise = ReadIEEE(fid, littleEndian);
 		subSize = subSize - 10;
 		posOfChannelLabels = ftell(fid);
		fread(fid, subSize, 'char');
		
 	else;
 		subSize = Read32Bits(fid, littleEndian);
 		chunkSize = chunkSize - subSize;
 		fread(fid, subSize, 'char');
 			
 	end;
end;

wordSize = floor((sampleSize + 7 ) / 8);
normalise = (2^16 - 1) / 2^(17 - wordSize * 8) / absoluteNormalise;
switch wordSize
	case 1, scale = normalise / 127.0;
	case 2, scale = normalise / 32768.0;
	case 4, scale = normalise / 32768.0 / 65536;
end
info.length = numSampleFrames / info.numWindowFrames;
signal = zeros(info.numChannels,info.length, info.numWindowFrames);
status = fseek(fid, soundPosition, 'bof');
for k = 1:info.numWindowFrames
	signal(:, :, k) = ReadWinFrame(fid, info.numChannels, info.length, ...
	  wordSize, littleEndian);
end
signal = signal .* scale;
info.dt = 1.0 / sampleRate;

chan = 1;
fseek(fid, posOfChannelLabels, -1);
for k = 1:info.numChannels / info.interleaveLevel
	labelValue = ReadIEEE(fid, littleEndian);
	for l = 1:info.interleaveLevel
		info.labels(chan) = labelValue;
		chan = chan + 1;
	end
end
status = fclose(fid);
