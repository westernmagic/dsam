/* This is a module specifiction file for the DataFile module */
mod_name		DataFile;
proc_routine	ReadSignal;
typedef struct {

	char	*name;			/* [MAX_FILE_PATH] */
	int		wordSize;
	int		endian;
	int		numChannels;
	double	defaultSampleRate;
	double 	duration;
	double	timeOffset;
	double	gain;
	double	normalisation;

} DataFile;
	
