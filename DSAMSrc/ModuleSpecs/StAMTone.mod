/* This is the module specification for the AMTone module
 */

mod_name		PureTone;
qualifier		AM;
proc_routine	GenerateSignal;
typedef struct {

	double	frequency;
	double	modulationFrequency;
	double	modulationDepth;
	double	intensity;
	double	duration;
	double	dt;

} AMTone;
