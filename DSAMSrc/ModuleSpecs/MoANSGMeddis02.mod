/* This is the module specification for the Meddis 2002 spike generation module
 */

mod_name		ANSpikeGen;
qualifier		Meddis02;
proc_routine	RunModel;
process_vars	true;
typedef struct {

	long	ranSeed;
	int		numFibres;
	double	pulseDuration;
	double	pulseMagnitude;
	double	refractoryPeriod;
	double	recoveryTau;

} Meddis02SG;