/* This is the module specification for the DRNL basilar
 * membrane module.
 */

mod_name		BasilarM;
qualifier		DRNL;
proc_routine	RunModel;

typedef struct {

	int			order;	
	double		nonLinearBroadeningCoeff;
	double		compressionPower;
	double		linearBroadeningCoeff;
	double		outputScale;
	CFListPtr	theCFs;

} BMDRNL;
