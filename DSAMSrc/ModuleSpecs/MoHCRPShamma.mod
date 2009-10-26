/* This is the module specification for the Shamma IHC receptor
 * potential module.
 */

mod_name		IHCRP;
qualifier		Shamma;
proc_routine	RunModel;

typedef struct {

	double	endocochlearPot_Et;	/* Endocochlear potential, mV. */
	double	reversalPot_Ek;		/* Reversal potential, mV. */
	double	reversalPotCorrection;	/* Rp/(Rt+Rp), mV. */
	double	totalCapacitance_C;	/* Total capacitance C = Ca + Cb, pF. */
	double	restingConductance_G0;	/* Resting Conductance, G0. */
	double	kConductance_Gk;	/* Potassium conductance, S (Seimens). */
	double	maxMConductance_Gmax;	/* Maximum mechanical conductance, S. */
	double	beta;				/* beta = exp(-G1/RT), dimensionless. */
	double	gamma;				/* gamma = Z1/RT, u/m. */
	double	ciliaTimeConst_tc;	/* BM/cilia displacement time constant, s. */
	double	ciliaCouplingGain_C;/* Cilia coupling gain. */
	double	referencePot;		/* Reference potential */

} Shamma;
