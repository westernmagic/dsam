/* This is the module specification for the AnAutoCorr module
 */

mod_name		Analysis;
qualifier		ACF;
proc_routine	Calc;
process_vars	true;
typedef struct {

	int		normalisationMode;
	double	timeOffset;
	double	timeConstant;
	double	period;

} AutoCorr;
