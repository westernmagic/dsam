/* This is the module specification for the harmonic stimulus
 * generation module.
 *
 */

mod_name		Harmonic;
proc_routine	GenerateSignal;
typedef struct {

	int		lowestHarmonic;
	int		highestHarmonic;
	int		mistunedHarmonic;
	int		order;
	NameSpecifier	phaseMode;
	long	ranSeed;
	double	mistuningFactor;
	double	frequency;
	double	intensity;
	double	duration, dt;
	double	modulationFrequency;
	double	modulationPhase;
	double	modulationDepth;
	double	lowerCutOffFreq;
	double	upperCutOffFreq;

} Harmonic;
