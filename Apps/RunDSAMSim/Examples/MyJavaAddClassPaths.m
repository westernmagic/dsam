function MyJavaAddClassPaths(paths)

currentPaths = javaclasspath();
numCurrentPaths = length(currentPaths);
for i = 1:length(paths)
	foundPath = false;
	for j = 1:numCurrentPaths
		if (strcmp(currentPaths{j}, paths{i}) == 1)
			foundPath = true;
		end
	end
	if (~foundPath)
		javaaddpath(paths{i});
	end
end

end

