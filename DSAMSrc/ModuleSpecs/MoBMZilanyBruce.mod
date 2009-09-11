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
	Float	s0;					/* parameter in Boltzman function */
	Float	x1;					/* parameter in Boltzman function */
	Float	s1;					/* parameter in Boltzman function */
	Float	shiftCP;			/* parameter in Boltzman function */
	Float	cutOffCP;			/* Cut-off frequency of control-path low-path filter (Hz) */
	int		lPOrder;			/* Order of the control-path low-pass filter */
	Float	dc;					/* Estimated dc shift of low-pass filter output at high level */
	Float	rC1;				/* Ratio of lower bound of tau_c1 to tau_C1max */
	Float	cOHC;				/* Scaling constant for impairment in the OHC. */
	int		nBorder;			/* Order of the narrow-band C1 filter (chirp). */
	Float	ohacsym;			/* Nonlinear asymmetry of OHC function. */
	Float	ihcasym;			/* Nonlinear asymmetry of IHC C1 transduction function */
	Float	aIHC0;				/* Constant in C1/C2 transduction function. */
	Float	bIHC;				/* Constant in C1/C2 transduction function. */
	Float	cIHC;				/* Scaling constant for impairment in the IHC. */
	Float	cutOffIHCLP;		/* IHC low-pass filter cut off frequency (Hz). */
	int		iHCLPOrder;			/* IHC low-pass filter order. */
	CFListPtr	cFList;			/* Pointer to centre frequency structure. */

} BMZB;

