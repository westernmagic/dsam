mod_name IHCRP;
qualifier Shamma3StateVelIn;
proc_routine RunModel;
process_vars true;

typedef struct {

	double	endocochlearPot_Et;	/* Endocochlear potential, mV. */
	double	reversalPot_Ek;		/* Reversal potential, mV. */
	double	reversalPotCorrection;	/* Rp/(Rt+Rp), mV. */
	double	totalCapacitance_C;	/* Total capacitance C = Ca + Cb, pF. */
	double	restingConductance_G0;	/* Resting Conductance, G0. */
	double	kConductance_Gk;	/* Potassium conductance, S (Seimens). */
	double	maxMConductance_Gmax;	/* Maximum mechanical conductance, S. */
	double	ciliaTimeConst_tc;	/* BM/cilia displacement time constant, s. */
	double	ciliaCouplingGain_C;    /* Cilia coupling gain. */
	double	referencePot;		/* Reference potential */
        double  sensitivity_s0;         /* Sensitivity constants for trans. function */
        double  sensitivity_s1;
        double  offset_x0;              /* Offset constants for transduction function */
        double  offset_x1;              
  	
} Sham3StVIn;

