function [path, file, ending] = str2FileName(str)
% str2FileName   convert a path string to [path, file, ending] strings.
%
% MATLAB Aimtools function to take a string and convert it 
% to path and file name, also ignoring any spaces at the end 
% of the string used for padding (UNIX version).
%
% [path, file, ending ] = str2FileName(string)
%	path : all characters before the last '/'
%	file: the name of the file itself (including endings)
%	ending (optional): the portion of string beyond the 
%	    last dot
%	string: the string to be split up.
%
% N.B. In order to store a array of strings of differing lengths
% it is necessary to pad them with zeros.

slashpos = [ max(findstr(str,'/')) max(findstr(str,'\')) ];
spacepos = min(findstr(str,' '));
   
dotpos = max(findstr(str,'.'));
if dotpos<slashpos
   dotpos = [];
end;

l = length(str);

if isempty(spacepos)
   spacepos = l+1;
end;

if isempty(slashpos)
   path = [];
   slashpos = 0;
else;
   path = str(1:slashpos);
end;

if nargout>2
   if ~isempty(dotpos)
      file = str(slashpos+1:dotpos-1);
      ending = str(dotpos+1:spacepos-1);
   else
      file = str(slashpos+1:spacepos-1);
      ending = [];
   end;
else
   file = str(slashpos+1:spacepos-1);
end;


   

   
   
   
