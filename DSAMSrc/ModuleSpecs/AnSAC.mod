/* This is the specification for the shuffled autocorrelogram
 * analysis module.
 */

mod_name		Analysis;
qualifier		SAC;
proc_routine	Calc;
process_vars	true;
typedef struct {

	int		order;			/* How many spikes away, or -1 for all spikes */
	double	eventThreshold;
	double	maxInterval;	/* Max. interval or -1 for end of signal. */
	double	binWidth;

} SAC;
