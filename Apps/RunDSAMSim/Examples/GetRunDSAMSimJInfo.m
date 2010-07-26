%
% This function returns a structure with all the RunDSAMSimJ signal information
% settings

function info = GetRunDSAMSimJInfo()

info.dt = RunDSAMSimJ.GetDt();
info.staticTimeFlag = RunDSAMSimJ.GetStaticTimeFlag();
info.length = RunDSAMSimJ.GetLength();
info.labels = RunDSAMSimJ.GetLabels();
info.numChannels = RunDSAMSimJ.GetNumChannels();
info.numWindowFrames = RunDSAMSimJ.GetNumWindowFrames();
info.outputTimeOffset = RunDSAMSimJ.GetOutputTimeOffset();
info.interleaveLevel = RunDSAMSimJ.GetInterleaveLevel();

end % function

