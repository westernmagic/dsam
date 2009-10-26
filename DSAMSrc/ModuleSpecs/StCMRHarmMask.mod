/* This is the module specification for the CMR with deterministic
 * harmonic masker module
 */

mod_name		CMR;
qualifier		HarmonicMasker;
proc_routine	GenerateSignal;
typedef struct {

	BoolSpecifier	binauralMode;
	NameSpecifier	spacingType;
	NameSpecifier	condition;
	NameSpecifier	onFreqEar;
	NameSpecifier	sigEar;
	NameSpecifier	sigMode;
	char	flankEar;
	int		nlow;		/* GetInt("Lower bands"); */
	int		nupp;		/* GetInt("Upper bands"); */
	int		nGapLow;
	int		nGapUpp;
	double	lowFBLevel;
	double	uppFBLevel;
	double	oFMLevel;
	double	spacing;
	double	mskmodfreq;
	double	sigLevel;
	double	sigFreq;
	double	gateTime;
	double	duration, dt;

} CMRHarmM;

