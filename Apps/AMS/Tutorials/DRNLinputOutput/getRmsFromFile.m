function outRMS=getRmsFromFile(AMSresultsFile)
%get resultsfrom file
fid = fopen(AMSresultsFile);
if fid == -1, error('AMS did not produce a file'),   end            
ignorFirstLine = fgetl(fid); 	%column headings (ignore)            
x= fscanf(fid,'%g %g',[2 inf]);
fclose(fid); delete results*
%all results now read

outRMS=x(2,1);