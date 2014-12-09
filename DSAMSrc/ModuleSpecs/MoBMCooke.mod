/* This is the module specification for the Cooke basilar
 * membrane module.
 */

mod_name		BasilarM;
qualifier		Cooke;
proc_routine	RunModel;
process_vars	true;

typedef struct {

	Float		broadeningCoeff;
	NameSpecifier outputMode;
	CFListPtr	theCFs;

} BM0Cooke;
