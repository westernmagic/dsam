/* This is the module specification for the multi-band pass pre-emphasis module.
 * The BPass structure has the form
 * typedef struct
 *		int		order;
 *		double	preAttenuation;
 *		double	upperCutOffFreq;
 *		double	lowerCutOffFreq;
 *	} BPassPars, *BPassParsPtr;
 *
 *** 10-11-98 Change to just using arrays? ***
 */

mod_name		PreEmphasis;
qualifier		MultiBPass;
proc_routine	RunModel;
typedef struct {

	int_al	numFilters;
	int		*order;
	double	*preAttenuation;
	double	*upperCutOffFreq;
	double	*lowerCutOffFreq;

} PEMultiBP;
