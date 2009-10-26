/* This is the module specification for the Meddis 86 IHC
 * synapse module.
 */

mod_name		IHC;
qualifier		Meddis86;
proc_routine	RunModel;

typedef struct {

	NameSpecifier   diagMode;	/* output variables during run (no/filename) */
	double	permConst_A;		/* Permeability constant (?). */
	double	permConst_B;		/* Units per second */
	double	releaseRate_g;		/* Release rate (units per second). */
	double	replenishRate_y;	/* Replenishment rate (units per second). */
	double	lossRate_l;			/* Loss rate (units per second). */
	double	reprocessRate_x;	/* Reprocessing rate (units per second). */
	double	recoveryRate_r;		/* Recovery rate (units per second). */
	double	maxFreePool_M;		/* Max. no. of transm. packets in free pool */
	double	firingRate_h;		/* Firing rate (spikes per second). */

} HairCell;
