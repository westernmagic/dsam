/* This is the module specification for the Find next Sample analysis module.
 * It can be used to find maximum or minimum indices
 */

mod_name		Analysis;
qualifier		FindNextIndex;
proc_routine	Calc;
typedef struct {

	NameSpecifier	mode;
	double	timeOffset;

} FindIndex;
