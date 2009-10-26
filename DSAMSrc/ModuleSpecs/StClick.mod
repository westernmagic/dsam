/* This is the module specification for the StClick module
 */

mod_name		Click;
proc_routine	GenerateSignal;
typedef struct {

	double	clickTime;
	double	amplitude;
	double	duration;
	double	dt;

} Click;
