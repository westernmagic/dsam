/* This is the module specification for the AnSAImage.c module.
 * It uses the UtStrobe.c utility module.
 */

mod_name		Analysis;
qualifier		SAI;
process_vars	true;
typedef struct {

	NameSpecifier	diagnosticMode;
	NameSpecifier	integrationMode;
	char		*strobeSpecification;
	Float		negativeWidth;
	Float		positiveWidth;
	Float		inputDecayRate;
	Float		imageDecayHalfLife;

} SAImage;
