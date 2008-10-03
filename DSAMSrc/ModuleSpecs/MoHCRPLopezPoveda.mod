/*
 * The hair-cell receptor potential from Lopez-Poveda E.A.,
 * Eustaquio-Martın A, (2006) "A biophysical model of the inner hair cell:
 * the contribution of potassium current to peripheral compression", 
 * J. Assoc. Res. Otolaryngol., 7, 218–235.
 * The fast and slow variables use the same variables as arrays, i.e. 
 * elements 0 and 1 are the 'Fast' and 'Slow' parameters respectively.
 * This reduces the number of parameter names.
 */

mod_name IHCRP;
qualifier LopezPoveda;
proc_routine RunModel;
process_vars true;

typedef struct {

	BoolSpecifier	bMCiliaCouplingMode;
	double	ciliaTC;	/* BM/cilia displacement time constant, s. */
	double	ciliaCGain;	/* Cilia coupling gain. */
	double	Et;			/* Endocochlear potential, Et (V). */
	double	Rp;			/* Epithelium resistance, Rp (Ohm). */
	double	Rt;			/* Epithelium resistance, Rt (Ohm). */
	double	gL;			/* Apical leakage conductance, gL (S). */
	double	Gm;			/* Maximum mechanical conductance, Gm (S). */
	double	s0;			/* Displacement sensitivity, s0 (1/m). */
	double	s1;			/* Displacement sensitivity, s1 (1/m). */
	double	u0;			/* Displacement offset, u0 (m). */
	double	u1;			/* Displacement offset, u1 (m). */
	double	Ca;			/* Apical capacitance, Ca (F). */
	double	Cb;			/* Apical capacitance, Cb (F). */
	int_al	numCurrents;	/* The number of currents. */
	WChar	**label;	/* Current label */
	double	*Ek;		/* Conductance reversal potential, Ek (V). */
	double	*G;			/* Maximum conductance, G (S). */
	double	*V1;		/* Conductance half-activation potential, V1 (V). */
	double	*VS1;		/* Conductance voltage sensitivity constant, S1 (V). */
	double	*V2;		/* Conductance half-activation potential, V2 (V). */
	double	*VS2;		/* Conductance voltage sensitivity constant, S2 (V). */
	double	*tau1Max;	/* Conductance voltage time constant, tau1 max (s). */
	double	*A1;		/* Conductance voltage time constant, A1 (s). */
	double	*B1;		/* Conductance voltage time constant, B1 (s). */
	double	*tau1Min;	/* Conductance voltage time constant, tau1Min (s). */
	double	*tau2Max;	/* Conductance voltage time constant, tau2Max (s). */
	double	*A2;		/* Conductance voltage time constant, A2 (s). */
	double	*B2;		/* Conductance voltage time constant, B2 (s). */
	double	*tau2Min;	/* Conductance voltage time constant, tau2Min (s). */
	double	VMRest;		/* Resting membrane potential (V). */

} LopezPoveda;

