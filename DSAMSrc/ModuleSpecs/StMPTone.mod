/* This is the module specification for the StMPTone module
 */

mod_name		PureTone;
qualifier		Multi;
proc_routine	GenerateSignal;
typedef struct {

	int_al	numPTones;
	double	*frequencies, *intensities, *phases;
	double	duration, dt;

} MPureTone;
