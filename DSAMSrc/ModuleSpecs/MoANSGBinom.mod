/* This is the module specification for the multi-fibre spike generation
 * module.
 */

mod_name		ANSpikeGen;
qualifier		Binomial;
proc_routine	RunModel;
typedef struct {

	int		numFibres;
	long	ranSeed;
	double	pulseDuration;
	double	pulseMagnitude;
	double	refractoryPeriod;

} BinomialSG;
