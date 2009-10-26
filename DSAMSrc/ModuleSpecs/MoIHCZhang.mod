/* This is the module specification for the Zhang et al. IHC
 * synapse module.
 */

mod_name		IHC;
qualifier		Zhang;
proc_routine	RunModel;
process_vars	true;

typedef struct {

	double	spont;		/* Spontaneous rate of fiber (spikes/s). */
	double	aAss;		/* Steady-state rate (spikes/s). */
	double	tauST;		/* Short-term time constant (s). */
	double	tauR;		/* Rapid time constant (s). */
	double	aAROverAST;	/* Rapid response amplitude to short-term response
						 * amplitude ratio. */
	double	pIMax;		/* Permeability at high sound level. */
	double	ks;			/* Species dependend voltage staturation parameter. */
	double	vs1;		/* Species dependend voltage staturation parameter. */
	double	vs2;		/* Species dependend voltage staturation parameter. */
	double	vs3;		/* Species dependend voltage staturation parameter. */

} ZhangHC;
