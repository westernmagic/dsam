/* This is the module specification for the FindBin analysis
 * module
 */

mod_name		Analysis;
qualifier		FindBin;
proc_routine	Calc;
typedef struct {

	NameSpecifier	mode;
	double	binWidth;
	double	timeOffset;
	double	timeWidth;

} FindBin;
