# This function closes the figure.
#
# usage DataXAxis(<info>)
#
# info -     info structure from DSAM AIFF data

function x = DataXAxis(info)

x = (info.outputTimeOffset:info.dt:info.outputTimeOffset+info.dt*(info.length-1))';

