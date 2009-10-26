/* This is the module specification for the McGregor neural cell 
 * model module.
 */

mod_name		Neuron;
qualifier		McGregor;
proc_routine	RunModel;
typedef struct {

	double	membraneTConst_Tm;	/* Membrane time constant (s). */	
	double	kDecayTConst_TGk;	/* Potassium conductance decay constant (s). */
	double	thresholdTConst_TTh;/* Threshold rise time constant (s). */
	double	accomConst_c;		/* Accomdation constant (dimensionless). */
	double	delayedRectKCond_b;	/* Delayed Rectifier K Cond. strength (ns). */
	double	restingThreshold_Th0;/* Resting threshold of cell (mV). */
	double	actionPotential;	/* Action potential of spike (mv). */
	double	kEquilibriumPot_Ek;	/* Reversal pot. of the K conductance (mV). */
	double	cellRestingPot_Er;	/* Resting potential for K conductance (mV). */

} McGregor;
