/* This is the module specification for the Zilany and Bruce IHC
 * synapse module.
 */

mod_name		IHC;
qualifier		ZilanyBruce;
proc_routine	RunModel;
process_vars	true;

typedef struct {

	double	spont;		/* Spontaneous rate of fiber (spikes/s). */
	double	Ass;		/* Steady-state rate (spikes/s). */
	double	tauST;		/* Short-term time constant (s). */
	double	tauR;		/* Rapid time constant (s). */
	double	Ar_Ast;		/* Rapid response amplitude to short-term response
						 * amplitude ratio. */
	double	PImax;		/* Permeability at high sound level. */

} ZBHC;

