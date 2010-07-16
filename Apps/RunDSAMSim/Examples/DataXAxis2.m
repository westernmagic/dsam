% This function calculates the t array using the DSAM info structure.
%
% usage DataXAxis(<outputTimeOffest><dt><len>)
%
% info -     info structure from DSAM AIFF data

function x = DataXAxis2(outputTimeOffset, dt, len)

x = (outputTimeOffset:dt:outputTimeOffset + dt * (len - 1))';


