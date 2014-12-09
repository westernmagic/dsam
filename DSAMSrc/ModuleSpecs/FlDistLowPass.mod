/* This is the module specification for the distributed
 * low pass filter module.
 */

mod_name		Filter;
qualifier		DLowPass;
proc_routine	RunModel;
process_vars	true;
typedef struct {

	NameSpecifier	mode;
	Float	signalMultiplier;	/* potential mV */
	ParArrayPtr	freqDistr;

} DLowPassF;

