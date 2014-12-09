/* This is the module specification for the AnCrossCorr module
 */

mod_name		Analysis;
qualifier		CCF;
proc_routine	Calc;
process_vars	true;
typedef struct {

	Float	timeOffset;
	Float	timeConstant;
	Float	period;

} CrossCorr;
