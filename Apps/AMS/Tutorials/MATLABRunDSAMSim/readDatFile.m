function [results, column1Values, columnHeadingsValues, errorMsg]=readDatFile(datFileName)
% readDatFile get results from .dat file created by AMS
% It is assumed that the .dat file consists of
%  a first row with short text followed by heading values
%  and later rows contain the results in a <time>  <data> <data> ...  format.

errorMsg=0;
try
    fid = fopen(datFileName);
catch
    errorMsg=1; return
end   

columnHeadings = fgetl(fid); 	%column headings     

% assume that the first 9 characters are text
closingParenthesis=1+ findstr(columnHeadings, ')');
columnHeadings=columnHeadings(closingParenthesis:end); %remove 'Time (s)'

% extract BFs
columnHeadingsValues=str2num(columnHeadings); 
numHeadings=length(columnHeadingsValues);

x= fscanf(fid,'%g',[numHeadings+1 inf]);
x=x';

column1Values=x(:,1);
results=x(:,2:end);

fclose(fid); 
