/* This is the module specification for the StMPPTone module
 */

mod_name		StepFunction;
proc_routine	GenerateSignal;
typedef struct {

	double	amplitude;
	double	beginEndAmplitude;
	double	beginPeriodDuration;
	double	endPeriodDuration;
	double	duration, dt;

} StepFun;
