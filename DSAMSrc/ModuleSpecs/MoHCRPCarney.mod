/* This is the module specification for the Laurel H. Carney IHC receptor
 * potential module.
 */

mod_name		IHCRP;
qualifier		Carney;
proc_routine	RunModel;

typedef struct {

	double		cutOffFrequency;	/* Fc */
	double		hCOperatingPoint;	/* PDihc */
	double		asymmetricalBias;	/* P0 */
	double		maxHCVoltage;		/* Vmax */
	double		waveDelayCoeff;		/* AD */
	double		waveDelayLength;	/* AL */
	double		referencePot;		/* Added for compatibility. */

} CarneyRP;
