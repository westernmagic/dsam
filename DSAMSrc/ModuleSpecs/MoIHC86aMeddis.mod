/* This is the module specification for the Meddis 86 IHC
 * synapse module.
 */

mod_name		IHC;
qualifier		Meddis86a;
proc_routine	RunModel;

typedef struct {

	double	maxFreePool_M;		/* Max. no. of transm. packets in free pool */
	double	replenishRate_y;	/* Replenishment rate (units per second). */
	double	lossRate_l;			/* Loss rate (units per second). */
	double	reprocessRate_x;	/* Reprocessing rate (units per second). */
	double	recoveryRate_r;		/* Recovery rate (units per second). */
	double	permeabilityPH_h;	/* Permeability constant. */
	double	permeabilityPZ_z;	/* Permeability constant. */
	double	firingRate_h2;		/* Permeability constant. */

} HairCell3;
