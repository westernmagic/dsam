/* This is the module specification for the ExpGatedTone module
 */

mod_name		ExpGatedTone;
qualifier		Stimulus;
proc_routine	GenerateSignal;
typedef struct {

	int		typeMode;
	int		floorMode;
	double	carrierFrequency;
	double	amplitude;
	double	phase;
	double	beginPeriodDuration;
	double	repetitionRate;
	double	halfLife;
	double	floor;
	double	duration;
	double	dt;

} EGatedTone;
