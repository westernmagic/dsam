/* This is the module specification for the DRNL basilar
 * membrane module.
 * This BM model
 */

mod_name		BasilarM;
qualifier		DRNL;
proc_routine	RunModel;

typedef struct {

	int				nonLinGTCascade;
	int				nonLinLPCascade;
	ParArray		nonLinBwidth;

	ParArray		comprScaleA;
	ParArray		comprScaleB;
	double			comprExponent;
	
	int				linGTCascade;
	int				linLPCascade;
	ParArray		linCF;
	ParArray		linBwidth;
	double			linScaleG;

	CFListPtr		theCFs;

} BMDRNL;
