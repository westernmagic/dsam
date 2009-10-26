/* This is the module specification for the Audio input module
 */

mod_name		IO;
qualifier		AudioIn;
proc_routine	ReadSignal;
process_vars	true;
typedef struct {

	int		deviceID;
	int		numChannels;
	int		segmentsPerBuffer;
	double	sampleRate;
	double	duration;
	double	sleep;
	double	gain;

} AudioIn;
