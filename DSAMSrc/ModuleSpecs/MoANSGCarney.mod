/* This is the module specification for the Carney spike generation module
 */

mod_name		ANSpikeGen;
qualifier		Carney;
proc_routine	RunModel;
process_vars	true;
typedef struct {

	NameSpecifier	diagnosticMode;
	NameSpecifier	inputMode; /* Use 'original' or 'DSAM' input mode. */
	long	ranSeed;
	int		numFibres;
	Float	pulseDuration;
	Float	pulseMagnitude;
	Float	refractoryPeriod;
	Float	maxThreshold;
	Float	dischargeCoeffC0;
	Float	dischargeCoeffC1;
	Float	dischargeTConstS0;
	Float	dischargeTConstS1;
	ParArrayPtr		distribution;

} CarneySG;
