/* This is the module specification for the Zhang et al. receptor potential
 * transduction process module.
 */

mod_name		IHCRP;
qualifier		Zhang;
proc_routine	RunModel;
process_vars	true;
typedef struct {

	double	a0;		/* Scaler in IHC nonlinear function. */
	double	b;		/* Parameter in IHC nonlinear function. */
	double	c;		/* Parameter in IHC nonlinear function. */
	double	d;		/* Parameter in IHC nonlinear function. */
	double	cut;	/* Cutoff frequency of IHC low-pass filter (Hz). */
	int		k;		/* Order of IHC lowe-pass filter. */

} IHCRPZhang;
