/* This is the module specification for the Simple gammatone basilar
 * membrane module.
 */

mod_name		BasilarM;
qualifier		GammaT;
proc_routine	RunModel;

typedef struct {

	int			cascade;	/* Defines the Q value of the gamma tone filters.*/
	CFListPtr	theCFs;		/* Pointer to centre frequency structure. */

} BMGammaT;
