/* This is the module specification for the Test DRNL basilar
 * membrane module.
 * Note that the simple array: compressionPar - has not been
 * correctly implemented in ModGen.
 */

mod_name		BasilarM;
qualifier		DRNL_Test;
proc_routine	RunModel;

typedef struct {

	int		nonLinGTCascade;
	int		nonLinLPCascade;
	NameSpecifier	lPFilterMode;
	NameSpecifier	compressionMode;
	double	/* * */compressionPars;
	int		linGTCascade;
	int		linLPCascade;
	double	linCF;
	double	linBwidth;
	double	linScaler;
	CFListPtr	theCFs;

} BM0DRNL;
