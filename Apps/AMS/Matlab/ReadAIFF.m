function signal=ReadAIFF(fname)
absoluteNormalise = 1.0;
fid=fopen(fname);
aiff_form=fread(fid,4,'char');
chunkSize=fread(fid,1,'int32');
aiff_aiff=fread(fid,4,'char');
chunkSize=chunkSize-16;

while chunkSize-4>0;
	chunkName=fread(fid,4,'char');
	chunkSize=chunkSize-4;
	if chunkName'==double('COMM');
% 		disp('step COMM');
 		subSize=fread(fid,1,'int32');
 		chunkSize=chunkSize-subSize;
 		numChannels=fread(fid,1,'int16');
 		subSize=subSize-2;
 		numSampleFrames=fread(fid,1,'int32');
 		subSize=subSize-4; 
 		sampleSize=fread(fid,1,'int16');
 		subSize=subSize-2;
 		sampleRate=ReadIEEE(fid);
 		subSize=subSize-10;
 		fread(fid,subSize,'char');
	elseif chunkName'==double('SSND');
% 		disp('step SSND');
 		subSize=fread(fid,1,'int32');
 		chunkSize=chunkSize-subSize;
 		offset=fread(fid,1,'int32');
		subSize=subSize-4;
		blockSize=fread(fid,1,'int32');
		subSize=subSize-4;
		soundPosition=ftell(fid)+offset;
		fread(fid,subSize,'char');
	elseif chunkName'==double('LUT2');
% 		disp('step LUT2');
 		subSize=fread(fid,1,'int32');
 		chunkSize=chunkSize-subSize;
 		interleaveLevel=fread(fid,1,'int16');
 		subSize=subSize-2;
  		numWindowFrames=fread(fid,1,'int16');
 		subSize=subSize-2;
  		staticTimeFlag=fread(fid,1,'int16');
 		subSize=subSize-2;
		outputTimeOffset=ReadIEEE(fid);
 		subSize=subSize-10;
 		absoluteNormalise=ReadIEEE(fid);
 		subSize=subSize-10;
 		fread(fid,subSize,'char');
		
 	else;
 		subSize=fread(fid,1,'int32');
 		chunkSize=chunkSize-subSize;
 		fread(fid,subSize,'char');
 			
 	end;
end;

wordSize=floor((sampleSize+7)/8);
normalise = (2^16 - 1) / 2^(17 - wordSize * 8) / absoluteNormalise;
switch wordSize
	case 1, scale = normalise / 127.0;
	case 2, scale = normalise / 32768.0;
	case 4, scale = normalise / 32768.0 / 65536;
end
frameLength = numSampleFrames / numWindowFrames;
signal = zeros(numChannels,frameLength,numWindowFrames);
status = fseek(fid,soundPosition,'bof');
for k = 1:numWindowFrames
	switch wordSize
		case 1, frame = fread(fid, [numChannels, frameLength],'char');
		case 2, frame = fread(fid, [numChannels, frameLength],'int16');
		case 4, frame = fread(fid, [numChannels, frameLength],'int32');
	end
	signal(:,:,k) = frame;
end
signal = signal .* scale;
status = fclose(fid);
