/* This is the module specification for the shape pulse module.
 * This module turns a train of events into a pulse train with set
 * magnitudes and durations.
 */

mod_name		Utility;
qualifier		ShapePulse;
typedef struct {

	double	eventThreshold;
	double	pulseDuration;
	double	pulseMagnitude;

} ShapePulse;
