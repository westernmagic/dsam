%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% File:			BitAnd.m
% Purpose:		Carries out a bit-wise and of two values.
% Comments:		The default binary format is big-endian, if this does not
%				work, then little-endian format is tried.
% Author:		L. P. O'Mard
% Revised by:
% Created:
% Updated:
% Copyright:	(c) 2002, University of Essex
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%

function result = BitAnd(a, b)

maxValue = max(a, b);
maxIndex = floor(log2(maxValue));
result = 0;
while (maxIndex >= 0)
	pow2 = 2^maxIndex;
	if ((a >= pow2) && (b >= pow2))
		result += pow2;
	endif
	if (a >= pow2)
		a -= pow2;
	endif
	if (b >= pow2)
		b -= pow2;
	endif
	maxIndex--;
endwhile
