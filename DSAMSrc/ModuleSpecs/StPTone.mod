/* This is a module specifiction file for the pure tone stimulus */
mod_name		PureTone;
proc_routine	GenerateSignal;
typedef struct {

	double	frequency;
	double	intensity;
	double	duration, dt;

} PureTone;
	
