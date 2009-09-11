/* This is the module specification for the Zilany and Bruce IHC
 * synapse module.
 */

mod_name		IHC;
qualifier		ZilanyBruce;
proc_routine	RunModel;
process_vars	true;

typedef struct {

	Float	spont;		/* Spontaneous rate of fiber (spikes/s). */
	Float	Ass;		/* Steady-state rate (spikes/s). */
	Float	tauST;		/* Short-term time constant (s). */
	Float	tauR;		/* Rapid time constant (s). */
	Float	Ar_Ast;		/* Rapid response amplitude to short-term response
						 * amplitude ratio. */
	Float	PImax;		/* Permeability at high sound level. */

} ZBHC;

