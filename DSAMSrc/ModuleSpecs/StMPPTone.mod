/* This is the module specification for the StMPPTone module
 */

mod_name		PureTone;
qualifier		MultiPulse;
proc_routine	GenerateSignal;
typedef struct {

	int_al	numPulses;
	double	*frequencies;
	double	intensity;
	double	beginPeriodDuration;
	double	pulseDuration;
	double	repetitionPeriod;
	double	duration, dt;

} PureTone4;
