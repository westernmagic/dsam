/* This is the module specification for the StWhiteNoise module
 */

mod_name		WhiteNoise;
proc_routine	GenerateSignal;
typedef struct {

	int		numChannels;
	int		correlationDegree;
	int		randomizationIndex;
	long	ranSeed;
	double	intensity;
	double	duration, dt;

} WhiteNoise;
