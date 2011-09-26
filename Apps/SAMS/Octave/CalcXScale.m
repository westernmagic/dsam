# This function closes the figure.
#
# usage CalcXScale(<info>)
#
# info -     info structure from DSAM AIFF data

function x = CalcXScale(info)

x = (info.outputTimeOffset:info.dt:info.outputTimeOffset+info.dt*(info.length-1))';

