/* This is the module specification for the Test DRNL basilar
 * membrane module.
 * Note that the simple array: compressionPar - has not been
 * correctly implemented in ModGen.
 */

mod_name		BasilarM;
qualifier		DRNL_Test;
proc_routine	RunModel;

typedef struct {

	int		order;
	NameSpecifier	compressionMode;
	double	narrowBroadeningCoeff;
	double	/* * */compressionPar;
	int		wideFOrder;
	double	wideFCentreFreq;
	double	wideFWidth;
	double	wideAttenuation;
	double	wideFLPCutOffScale;
	double	outputScale;
	CFListPtr	theCFs;

} BM0DRNL;
