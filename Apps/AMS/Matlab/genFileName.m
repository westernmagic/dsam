function name = genFileName(stra,strb)
% genFileName    find a new file name by choosing the 'run number'
%
% name = genFileName(stra,strb)
%  	name: output file name
%	stra: first part of string
%	strb: second part of string
%
% The function works by finding the file name of the format
% <stra> '_' <run_number> <strb> of the minimum run_number
% for which there is no file presently.

i=0;
carryon =1;
while carryon
   i=i+1;
   file = [stra '_' num2str(i) strb];
   if ~isAFile(file)
      carryon = 0;
   end;
end;
name = file;
