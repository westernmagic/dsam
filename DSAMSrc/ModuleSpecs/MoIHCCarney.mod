/* This is the module specification for the Laurel H. Carney IHC
 * synapse module.
 */

mod_name		IHC;
qualifier		Carney;
proc_routine	RunModel;

typedef struct {

	double	maxHCVoltage;		/* Vmax */
	double	restingReleaseRate;	/* R0 */
	double	restingPerm;		/* Prest */
	double	maxGlobalPerm;		/* PGmax */
	double	maxLocalPerm;		/* PLmax */
	double	maxImmediatePerm;	/* PImax */
	double	maxLocalVolume;		/* VLmax */
	double	minLocalVolume;		/* VLmin */
	double	maxImmediateVolume;	/* VImax */
	double	minImmediateVolume;	/* VImin */


} CarneyHC;
