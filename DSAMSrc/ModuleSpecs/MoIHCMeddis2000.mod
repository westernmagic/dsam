/* This is the module specification for the Meddis 2000 IHC
 * synapse module.
 */

mod_name		IHC;
qualifier		Meddis2000;
proc_routine	RunModel;

typedef struct {

	NameSpecifier	opMode;	/* output probabiliy mode: spikes or probability */
	NameSpecifier   diagMode;	/* output variables during run (no/filename) */
	double 	ranSeed;		/* seed for random number generator */

	double recPotOffset;		/* Voltage correction for receptor potential */

	double CaVrev;			/* Calcium reversal potential */
	double betaCa; 			/* Boltzman function parameters */
	double gammaCa;			/* Boltzman function parameters */ 

	double pCa;  			/* Calcium current exponent */ 
	double GCaMax;			/* Maximum calcium conductance */  
	double perm_Ca0;  		/* Permeabilituy with zero Calcium concentration */
  	double perm_z;			/* Permeability constant. */
        
	double tauCaChan;		/* Time constant for the calcium channel*/        
	double tauConcCa;		/* Time constant for the calcium concentration */

	int	maxFreePool_M;		/* Max. no. of transm. packets in free pool */
	double	replenishRate_y;	/* Replenishment rate (units per second). */
	double	lossRate_l;		/* Loss rate (units per second). */
	double	reprocessRate_x;	/* Reprocessing rate (units per second). */
	double	recoveryRate_r;		/* Recovery rate (units per second). */


} HairCell4;
