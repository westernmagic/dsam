%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% File:			ReadWinFrame2.m
% Purpose:		Reads a window from from an AIFF file.
% Comments:	
% Author:		L. P. O'Mard
% Revised by:	M.Tsuzaki (ATR SLT)
% Created:
% Updated:		12 Jul 2002
% Copyright:	(c) 2000, University of Essex
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%

function signal=ReadWinFrame(fid, frameId, info)

p_bias = ftell(fid);

if info.numWindowFrames == 1
	top = frameId(1);
	if length(frameId) > 1
		bot = size(frameId, 2);
	else
		bot = info.length;
	end
else
	frameId=frameId(find((frameId >= 1) & frameId<=info.numWindowFrames));
end

if isempty(frameId)
    warning(sprintf('FrameID should be in the range of [1 %d]', ...
	  info.numWindowFrames));
    signal = [];
    return
end

switch info.wordSize
	case 1
	   if info.numWindowFrames == 1
		fseek(fid,top-1,seek_cur);
		nn = bot - top + 1;
		signal = fread(fid, [info.numChannels, nn], 'char');
	   else
		for kk=1:length(frameId)
		    fseek(fid, p_bais + (frameId(kk) - 1) * info.numChannels * ...
			  info.length, 'bof');
		    signal(:,:,kk) = fread(fid, [info.numChannels, info.length], ...
			  'char');
		end
	   end
	case 2
	   if info.numWindowFrames == 1
		fseek(fid,2*(top-1),'cof');
		nn = bot - top + 1;
		signal = fread(fid,[info.numChannels, nn], 'short');
	   else
		for kk=1:length(frameId)
		    fseek(fid,p_bias + 2 * (frameId(kk) - 1) * info.numChannels * ...
			  info.length, 'bof');
		    signal(:,:,kk) = fread(fid, [info.numChannels, info.length], ...
			  'int16');
		end
	   end
	case 4
	   if info.numWindowFrames == 1
		fseek(fid,4 * (top - 1), 'cof');
		nn = bot - top + 1;
		signal = fread(fid,[info.numChannels, nn], 'uint32');
	   else
		for kk = 1:length(frameId)
		    fseek(fid, p_bias + 4 * (frameId(kk) - 1) * info.numChannels * ...
			  info.length, 'bof');
		    signal(:,:,kk) = fread(fid, [info.numChannels, info.length], ...
			  'uint32');
		end
	   end
	
end
