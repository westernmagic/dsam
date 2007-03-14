/* This is the module specification for the harmonic stimulus
 * generation module.
 *
 */

mod_name		Harmonic;
qualifier 		FreqSwept;
proc_routine	GenerateSignal;
process_vars	true;
typedef struct {

	int		lowestHarmonic;
	int		highestHarmonic;
	NameSpecifier		phaseMode;
	double	phaseVariable;
	double	initialFreq;
	double	finalFreq;
	double	intensity;
	double	duration, dt;

} FSHarm;
