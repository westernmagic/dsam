%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% File:			ReadAIFF.m
% Purpose:		Reads an AIFF format file.
% Comments:		The default binary format is big-endian, if this does not
%				work, then little-endian format is tried.
% Author:		L. P. O'Mard
% Revised by:	M.Tsuzaki
% Created:
% Updated:
% Copyright:	(c) 2000, University of Essex
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [signal, info] = ReadAIFF(fname,frameId)

if nargin < 2
	frameId=1:1000000;
end

fid = fopen(fname, 'rb');
if (fid < 0)
	error(['Could not find file: ' fname]);
end
littleEndian = 0;
aiff_form = ReadBytes(fid, 4, littleEndian);
if aiff_form' == toascii('FORM')
	%disp('big-endian');
else
	%disp('little-endian');
	littleEndian = 1;
end;

chunkSize = Read32Bits(fid, littleEndian);

aiff_aiff = ReadBytes(fid, 4, littleEndian);
if aiff_aiff' ~= toascii('AIFF')
	aiff_aiff;
	error('Not a valid AIFF file.');
end;
chunkSize = chunkSize - 16;

info.numWindowFrames = 1;
info.outputTimeOffset = 0.0;
info.staticTimeFlag = 0;
absoluteNormalise = 1.0;
posOfChannelLabels = 0;

while chunkSize - 4 > 0;
	chunkName = ReadBytes(fid, 4, littleEndian);
	chunkSize = chunkSize - 4;
	if chunkName' == toascii('COMM');
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
		fseek(fid, subSize, 'cof');
	elseif chunkName' == toascii('SSND');
		%disp('step SSND');
		subSize = Read32Bits(fid, littleEndian);
		chunkSize = chunkSize - subSize;
		offset = Read32Bits(fid, littleEndian);
		subSize = subSize - 4;
		blockSize = Read32Bits(fid, littleEndian);
		subSize = subSize - 4;
		soundPosition = ftell(fid) + offset;
		fseek(fid, subSize, 'cof');
	elseif chunkName' == toascii('LUT2');
		%disp('step LUT2');
		subSize = Read32Bits(fid, littleEndian);
		chunkSize = chunkSize - subSize;
		info.interleaveLevel = Read16Bits(fid, littleEndian);
		subSize = subSize - 2;
		info.numWindowFrames = Read16Bits(fid, littleEndian);
		subSize = subSize - 2;
		info.staticTimeFlag = Read16Bits(fid, littleEndian);
		subSize = subSize - 2;
		info.outputTimeOffset = ReadIEEE(fid, littleEndian);
		subSize = subSize - 10;
		absoluteNormalise = ReadIEEE(fid, littleEndian);
		subSize = subSize - 10;
		posOfChannelLabels = ftell(fid);
		fseek(fid, subSize, 'cof');
	else;
		subSize = Read32Bits(fid, littleEndian);
		chunkSize = chunkSize - subSize;
		fseek(fid, subSize, 'cof');
	end;
end;

info.wordSize = floor((sampleSize + 7 ) / 8);
info.length = numSampleFrames / info.numWindowFrames;
info.dt = 1.0 / sampleRate;
normalise = (2^16 - 1) / 2^(17 - info.wordSize * 8) / absoluteNormalise;
switch info.wordSize
	case 1, scale = normalise / 127.0;
	case 2, scale = normalise / 32768.0;
	case 4, scale = normalise / 32768.0 / 65536;
end
%if (info.numWindowFrames > 1)
%	disp('This verions currently only handles single frame signals.');
%	info.numWindowFrames = 1;
%endif
fclose(fid);

if littleEndian
	fid = fopen(fname,'r','l');
%	disp('open as little endian')
else
	fid = fopen(fname,'rb','b');
%	disp('open as big endian')
end

status = fseek(fid, soundPosition, 'bof');
signal = ReadWinFrame(fid, frameId, info);
signal = signal .* scale;

if (posOfChannelLabels)
	chan = 1;
	fseek(fid, posOfChannelLabels, 'bof');
	for k = 1:info.numChannels / info.interleaveLevel
		labelValue = ReadIEEE(fid, littleEndian);
		for l = 1:info.interleaveLevel
			info.labels(chan) = labelValue;
			chan = chan + 1;
		end
	end
end
status = fclose(fid);

