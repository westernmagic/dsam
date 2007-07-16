# This function sets the print output.

function SetPrintOutput(print)

	switch print
		case 1
			gset output "|lpr -Plp";
		case 2
			gset output "|lpr -Plpepson";
		case 3
			gset output "foo.ps";
		case 4
			printf("Output to specified file\n");
		otherwise
	end
