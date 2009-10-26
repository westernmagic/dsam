/* This is the module specification for the PulseTrain stimulus module
 */

mod_name		PulseTrain;
proc_routine	GenerateSignal;
typedef struct {

	double	pulseRate;
	double	pulseDuration;
	double	amplitude;
	double	duration;
	double	dt;

} PulseTrain;
