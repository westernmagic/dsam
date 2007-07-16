# This function displays the data from the filter investigation program.

function AROGammaC800(print)

	dataRef = "GammaC800";

	if (print != 0)
		printf("Output sent to printer\n");
	endif

	############################################################################
	figure(1);			########################################################
	############################################################################
	clearplot;
	PrintFigure(print);
	if (print == 4)
		gset output "figGammaC800_1.gif";
	endif

	gset autoscale;
	gset ytics 10
	gset xtics autofreq
	DispTCurve([dataRef, "/", "tcurve.dat"], dataRef);

	if (print != 0)
		closeplot;
	endif


	############################################################################
	figure(2);			########################################################
	############################################################################

	clearplot;
	clearplot;
	gset autoscale;
	gset ytics autofreq
	gset yrange [1:100];
	DispIOFuncs([dataRef, "/", "iofunc.dat"], dataRef, 1);

	PrintFigure(print);
	if (print == 4)
		gset output "figGammaC800_2.gif";
	endif

	if (print != 0)
		replot;
		closeplot;
	endif

	############################################################################
	figure(3);			########################################################
	############################################################################
	clearplot;

	gset autoscale;
	gset yrange [1:100];
	if (print == 4)
		#gset xrange [0.0:2100.0];
	endif
	DispFShape([dataRef, "/", "fshape.dat"], dataRef, 0);

	PrintFigure(print);
	if (print == 4)
		gset output "figGammaC800_3.gif";
		gset logscale x
		gset logscale y
	endif

	if (print != 0)
		replot;
		closeplot;
	endif

	############################################################################
	figure(4);			########################################################
	############################################################################
	clearplot;

	gset autoscale;
	gset xtics 500
	gset timestamp bottom;
	gset ytics autofreq;
	DispPFFuncs([dataRef, "/", "pffunc.dat"], dataRef);

	PrintFigure(print);
	if (print == 4)
		gset output "figGammaC800_4.gif";
	endif

	if (print != 0)
		replot;
		closeplot;
	endif

	############################################################################
	figure(5);			########################################################
	############################################################################
	clearplot;

	gset autoscale;
	#gset xtics 5000
	#gset yrange [-3.0:2.0];
	gset xrange [200:1300];
	DispRPFFuncs([dataRef, "/", "rpffunc.dat"], dataRef);

	PrintFigure(print);
	if (print == 4)
		gset output "figGammaC800_5.gif";
	endif

	if (print != 0)
		replot;
		closeplot;
	endif

	############################################################################
	figure(6);			########################################################
	############################################################################
	clearplot;

	gset autoscale;
	gset key bottom;
	gset xtics autofreq;
	gset ytics autofreq;
	DispTTSRatio([dataRef, "/", "ttsfunc.dat"], dataRef);

	PrintFigure(print);
	if (print == 4)
		gset output "figGammaC800_6.gif";
	endif

	if (print != 0)
		replot;
		closeplot;
	endif

	############################################################################
	figure(7);			########################################################
	############################################################################
	clearplot;

	gset autoscale;
	gset ytics autofreq
	gset xrange [0.030:0.035]
	gset xtics 0.001
	DispTTSResp([dataRef, "/", "ttsrfunc.dat"], dataRef);

	PrintFigure(print);
	if (print == 4)
		gset output "figGammaC800_7.gif";
	endif

	if (print != 0)
		replot;
		closeplot;
	endif

	############################################################################
	figure(8);			########################################################
	############################################################################
	clearplot;

	gset autoscale;
	gset timestamp bottom;
	gset ytics autofreq;
	gset xtics autofreq;
	gset xrange [0.02:0.04];
	DispImpResp([dataRef, "/", "irfunc.dat"], 10000, dataRef);

	PrintFigure(print);
	if (print == 4)
		gset output "figGammaC800_8.gif";
	endif

	if (print != 0)
		replot;
		closeplot;
	endif

	############################################################################
	figure(9);			########################################################
	############################################################################
	clearplot;

	PrintFigure(print);
	if (print == 4)
		gset output "figGammaC800_9.gif";
	endif

	gset autoscale;
	gset xtics 400;
	gset xrange [0:1500]
	gset yrange [1.0e5:1.0e7]
	DispDProds([dataRef, "/", "dpfunc.dat"], dataRef);

	if (print != 0)
		closeplot;
	endif

endfunction
