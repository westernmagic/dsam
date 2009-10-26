/* This is the module specification for the AnSAImage.c module.
 * It uses the UtStrobe.c utility module.
 */

mod_name		Analysis;
qualifier		SAI;
typedef struct {

	NameSpecifier	diagnosticMode;
	NameSpecifier	integrationMode;
	char		*strobeSpecification;
	double		negativeWidth;
	double		positiveWidth;
	double		inputDecayRate;
	double		imageDecayHalfLife;

} SAImage;
