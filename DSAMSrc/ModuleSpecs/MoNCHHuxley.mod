/* This is the module specification for the Ray Meddis' implementation
 * of the Hudgkin Huxley module.
 * The parameters appear in the order that the equation was originally written.
 */

mod_name		Neuron;
qualifier		HHuxley;
proc_routine	RunModel;
typedef		int	IonChanListPtr;

typedef struct {

	NameSpecifier	diagnosticMode;
	NameSpecifier	operationMode;
	NameSpecifier	injectionMode;
	double			excitatoryReversalPot;
	double			inhibitoryReversalPot;
	double			cellCapacitance;
	double			restingPotential;
	double			restingSignalDuration;
	double			restingCriteria;
	IonChanListPtr	iCList;
	
	/* Private members */

} HHuxleyNC;
