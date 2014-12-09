/* This is the module specification for the band pass pre-emphasis module.
 */

mod_name		Filter;
qualifier		BandPass;
proc_routine	RunModel;
process_vars	true;
typedef struct {

	int		cascade;
	Float	preAttenuation;			/* Pre-attenuation for filter. */
	Float	upperCutOffFreq, lowerCutOffFreq;

}  BandPassF;
