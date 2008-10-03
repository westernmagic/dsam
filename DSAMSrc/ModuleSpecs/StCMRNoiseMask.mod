/* This is the module specification for the CMR with stochastic masker module
 */

mod_name		CMR;
qualifier		NoiseMasker;
proc_routine	GenerateSignal;
process_vars	true;
typedef struct {

	BoolSpecifier	binauralMode;
	NameSpecifier	spacingType;
	NameSpecifier	condition;
	NameSpecifier	onFreqEar;
	NameSpecifier	sigEar;
	char	flankEar;
	int		nlow;		/* GetInt("Lower bands"); */
	int		nupp;		/* GetInt("Upper bands"); */
	int		nGapLow;
	int		nGapUpp;
	long	ranSeed;
	double	lowFBLevel;
	double	uppFBLevel;
	double	oFMLevel;		/* GetDoubleAs("OFM level","ratio") */
	double	spacing;
	double	maskerModFreq;
	double	bandwidth;
	double	sigLevel;
	double	sigFreq;
	double	gateTime;
	double	duration, dt;

} CMRNoiseM;