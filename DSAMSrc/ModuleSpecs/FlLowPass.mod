/* This is the module specification for the dendrite LP 
 * model module.
 */

mod_name		Filter;
qualifier		DendriteLP;
proc_routine	RunModel;
typedef struct {

	int		mode;
	double	cutOffFrequency;
	double	signalMultiplier;	/* potential mV */

} LowPassF;
