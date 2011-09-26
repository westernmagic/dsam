function newstr = rmParInStr(par,str)
% rmParInStr     remove specifed parameter from a AIMtools format string.
%
% MATLAB Aimtools function to find the value of a parameter
% in a string name conforming to aimtools naming rules and
% remove that parameter.
% 
% newstr = rmParInStr(par,str)
%	par: string of characters describing the parameter
%	str: input string
%	newstr: the new string with the par removed.
% 
% N.B. standard parameter format is:
% <non-alphabetic char> <alpha string> <numeric string> 
% <nonnumeric char>
% help aimtools for more information

% Find the relevant string portion
pospar = findstr(str,par);
l=length(str);

newstr=str;

% Check to find any that conform to standard
for i=1:length(pospar)
   start = pospar(i);
   fin = start + length(par)-1;
   
   if (start==1 | ~isletter(str(start-1)) | (par=='_' & ~isletter(str(start+1))) ) & fin<l 
      if isdigit(str(fin+1))
         % If gets to here then it does conform to the rules
         % so get the whole numeric portion and convert
         
         % Find the next non-numeric
         j=fin+1;
         while j<=l & isdigit(str(j)) 
            j=j+1;
         end;
         
         % Convert all connected characters to a number        	 
	 newstr = [str(1:start-1) str(j:l)];
      end;
   end;
end;

