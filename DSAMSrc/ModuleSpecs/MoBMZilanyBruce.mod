/* This is the module specification for the Zhang et al. basilar
 * membrane module.
 */

mod_name		BasilarM;
qualifier		ZilanyBruce;
proc_routine	RunModel;
process_vars	true;
typedef struct {

	NameSpecifier	outputMode;		/* Mode to use. */
	NameSpecifier	microPaInput;	/* Expect input in micro Pascals. */
	int		wborder;			/* Order of the wide-band gammetone filter */
	double	s0;					/* parameter in Boltzman function */
	double	x1;					/* parameter in Boltzman function */
	double	s1;					/* parameter in Boltzman function */
	double	shiftCP;			/* parameter in Boltzman function */
	double	cutOffCP;			/* Cut-off frequency of control-path low-path filter (Hz) */
	int		lPOrder;			/* Order of the control-path low-pass filter */
	double	dc;					/* Estimated dc shift of low-pass filter output at high level */
	double	rC1;				/* Ratio of lower bound of tau_c1 to tau_C1max */
	double	cOHC;				/* Scaling constant for impairment in the OHC. */
	int		nBorder;			/* Order of the narrow-band C1 filter (chirp). */
	double	ohacsym;			/* Nonlinear asymmetry of OHC function. */
	double	ihcasym;			/* Nonlinear asymmetry of IHC C1 transduction function */
	double	aIHC0;				/* Constant in C1/C2 transduction function. */
	double	bIHC;				/* Constant in C1/C2 transduction function. */
	double	cIHC;				/* Scaling constant for impairment in the IHC. */
	double	cutOffIHCLP;		/* IHC low-pass filter cut off frequency (Hz). */
	int		iHCLPOrder;			/* IHC low-pass filter order. */
	CFListPtr	cFList;			/* Pointer to centre frequency structure. */

} BMZB;

