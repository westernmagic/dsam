% This function calculates the t array using the DSAM info structure.
%
% usage DataXAxis(<info>)
%
% info -     info structure from DSAM AIFF data

function x = DataXAxis(info)

x = (info.outputTimeOffset:info.dt:info.outputTimeOffset+info.dt*(info.length-1))';

