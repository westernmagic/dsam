/* This is the module specification for the spike generation module
 */

mod_name		ANSpikeGen;
qualifier		Simple;
proc_routine	RunModel;
process_vars	true;
typedef struct {

	long	ranSeed;
	int		numFibres;
	double	pulseDuration;
	double	pulseMagnitude;
	double	refractoryPeriod;
	ParArrayPtr		distribution;

} SimpleSG;
