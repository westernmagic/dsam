/* This is the module specification for the Gammachirp module
 * membrane module.
 */

mod_name		BasilarM;
qualifier		GammaChirp;
proc_routine	RunModel;

typedef struct {

	NameSpecifier	diagnosticMode;	/* Mode for debug/diagnostics output */
	NameSpecifier	opMode; 			/* SwCntl */
	int	cascade;		/* n */
	double	bCoeff;		/* b */
	double	cCoeff0;	/* cCoef(0) */
	double	cCoeff1;	/* cCoef(1) */
	double	cLowerLim;	/* cLim(0) */
	double	cUpperLim;	/* cLim(1) */

	CFListPtr	theCFs;	/* Pointer to centre frequency structure. */

} BMGammaC;
