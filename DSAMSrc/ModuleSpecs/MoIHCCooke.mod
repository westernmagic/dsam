/* This is the module specification for the Laurel H. Carney IHC
 * synapse module.
 */

mod_name		IHC;
qualifier		Cooke91;
proc_routine	RunModel;

typedef struct {

	double	crawfordConst;		/* Crawford and Fettiplace c Value */
	double	releaseFraction;	/* Release fraction. */
	double	refillFraction;		/* Replenishment fraction. */
	double	spontRate;			/* desired spontaneous firing rate. */
	double	maxSpikeRate;		/* maximum possible firing rate. */

} CookeHC;
