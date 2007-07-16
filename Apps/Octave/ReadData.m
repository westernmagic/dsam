# This routine reads an xy data file.
#
# usage [var] = ReadData(<fileName>)


function data = ReadData(fileName)

	printf("Reading the file '%s'\n", fileName);
	[myFile, msg] = fopen(fileName, "r");
	if (myFile == -1)
		printf("Error: %s\n", msg);
		return
	endif
	newData = fscanf(myFile, "%g %g", [2, Inf]);
	data = newData';

endfunction
