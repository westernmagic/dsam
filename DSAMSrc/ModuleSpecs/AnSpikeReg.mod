/* This is the specification for the Spike interval regularity analysis module.
 */

mod_name		Analysis;
qualifier		SpikeRegularity;
proc_routine	Calc;
typedef struct {

	NameSpecifier	outputMode;
	double	eventThreshold;
	double	windowWidth;
	double	timeOffset;
	double	timeRange;
	double	deadTime;
	double	countThreshold;

} SpikeReg;
