/* This is the module specification for the FMTone module
 */

mod_name		PureTone;
qualifier		FM;
proc_routine	GenerateSignal;
typedef struct {

	double	frequency;
	double	intensity;
	double	duration;
	double	dt;
	double	phase;
	double	modulationDepth;
	double	modulationFrequency;
	double	modulationPhase;

} FMTone;
