/* This is the module specification for the Carney spike generation module
 */

mod_name		ANSpikeGen;
qualifier		Carney;
proc_routine	RunModel;
typedef struct {

	long	ranSeed;
	int		numFibres;
	double	pulseDuration;
	double	pulseMagnitude;
	double	refractoryPeriod;
	double	maxThreshold;
	double	dischargeCoeffC0;
	double	dischargeCoeffC1;
	double	dischargeTConstS0;
	double	dischargeTConstS1;

} CarneySG;
