/* This is the module specification for the FIR filter module.
 */

mod_name		Filter;
qualifier		FIR;
proc_routine	RunProcess;
typedef struct {

	NameSpecifier	type;
	int		numTaps;
	int_al	numBands;
	double	*bandFreqs, *desired, *weights;

}  FIR;
