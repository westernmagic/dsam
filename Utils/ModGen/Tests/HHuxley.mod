/* HHuxley module specification for modgenerator 1.2
 *
 * 15 Nov 95
 */

mod_name		NC;
qualifier		HHuxley;
proc_routine	RunModel;

typedef int Boolean;
/* typedef struct ionChan_HHuxley IonChan_HHuxley;
 * typedef struct gate_HHuxley Gate_HHuxley;
 *
 * Current modgenerator doesn't seem to understand the above.
 */

typedef struct {

	int		hHFormat;		/* really Boolean */
	
	double	eLeak;
	double	gLeak;

	int_al	nInputs;		/* really unsigned int */
	double	*eInput, *gInput;

	int_al	nIonChans;		/* really a second int_al */
	double	*ionChans;	/* really (IonChan_HHuxley *) */

	int_al	nGates;			/* really another int_al */
	double	*gates;		/* really (Gate_HHuxley *) */

} HHuxley;
/* modgenerator dumps core if "HHAuto" above is replaced with "HHuxley" */
