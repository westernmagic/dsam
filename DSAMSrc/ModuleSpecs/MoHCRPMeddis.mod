/* This is the module specification for the Meddis IHC receptor
 * potential module.
 */

mod_name		IHCRP;
qualifier		Meddis;
proc_routine	RunModel;

typedef struct {

	double	permConst_A;		/* For K+ probability: base Amplitude (uPa). */
	double	permConst_B;		/* For K+ probability: amplitude scale (/uPa) */
	double	releaseRate_g;		/* Release rate (units per second). */
	double	mTimeConst_tm;		/* Membrane time constant, s. */

} MeddisRP;
