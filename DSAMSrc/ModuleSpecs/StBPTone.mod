/* This is the module specification for the BPTone module
 */

mod_name		PureTone;
qualifier		Binaural;
proc_routine	GenerateSignal;
typedef struct {

	double	leftFrequency;
	double	rightFrequency;
	double	leftIntensity;
	double	rightIntensity;
	double	phaseDifference;
	double	duration;
	double	dt;

} BPureTone;
