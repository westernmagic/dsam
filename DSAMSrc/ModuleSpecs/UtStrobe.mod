/* This is the module specification for the UtStrobe.c module
 * It generates strobe points to be used by the AnSAImage module.
 */

mod_name		Utility;
qualifier		Strobe;
typedef struct {

	int		typeMode;
	int		diagnosticMode;
	double	threshold;
	double	thresholdDecayRate;
	double	delay;
	double	delayTimeout;

} Strobe;

