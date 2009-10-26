/* This is the module specification for the Arle/Kim version of the
 * McGregor model module.
 */

mod_name		Neuron;
qualifier		ArleKim;
proc_routine	RunModel;
typedef struct {

	double	membraneTConst_Tm;	/* Membrane time constant (s). */	
	double	kDecayTConst_TGk;	/* Potassium conductance decay constant (s). */
	double	thresholdTConst_TTh;/* Threshold rise time constant (s). */
	double	accomConst_c;		/* Accomdation constant (dimensionless). */
	double	delayedRectKCond_b;	/* Delayed Rectifier K Cond. strength (ns). */
	double	restingThreshold_Th0;/* Resting threshold of cell (mV). */
	double	actionPotential;	/* Action potential of spike (mv). */
	double	nonLinearVConst_Vnl;/* Non-linear voltage constant (mV). */
	double	kReversalPoten_Ek;	/* Reversal pot. of the K conductance (mV). */
	double	bReversalPoten_Eb;	/* Rev. pot. of all other conductances (mV) */
	double	kRestingCond_gk;	/* Resting potassium conductance (nS). */
	double	bRestingCond_gb;	/* Resting component for all other conds.(nS).*/	
	

} ArleKim;
