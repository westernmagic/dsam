function yesno = isAFile(fname)
% isAFile        determines if a file exists
%
%   AIMTools function to determine if the specified file exists
%
%     y = isAFile(fname)
%   Returns a '1' if the file 'fname' exists

[p f] = str2FileName(fname);
fname = [p f];

fid = fopen(slash4OS(fname),'r');

if (fid>0)
   yesno = 1;
   fclose(fid);
else
   yesno = 0;
end;
