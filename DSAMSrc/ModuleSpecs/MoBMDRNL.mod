/* This is the module specification for the DRNL basilar
 * membrane module.
 * This BM model
 */

mod_name		BasilarM;
qualifier		DRNL;
proc_routine	RunModel;

typedef struct {

	int		nonLinGTCascade;
	int		nonLinLPCascade;

	double		compressionPower;
	
	int		linGTCascade;
	int		linLPCascade;
	int		linCFMode;
	double	linCFPars;
	int		linBandwidthMode;
	double	linScale;						/* b */

	CFListPtr	theCFs;

} BMDRNL;
