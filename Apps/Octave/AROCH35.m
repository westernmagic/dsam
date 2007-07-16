# This function displays the data from the filter investigation program.

function AROCH35(print)

	dataRef = "DRNL800";

	if (print != 0)
		printf("Output sent to printer\n");
	endif

	############################################################################
	figure(1);			########################################################
	############################################################################
	clearplot;
	PrintFigure(print);
	if (print == 4)
		gset output "figDRNL800_1.gif";
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
	gset autoscale;
	gset xtics autofreq;
	gset ytics autofreq;
	DispIOFuncs([dataRef, "/", "iofunc.dat"], dataRef, 1);

	PrintFigure(print);
	if (print == 4)
		gset output "figDRNL800_2.gif";
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
	gset xtics autofreq;
	gset ytics autofreq;
	DispFShape([dataRef, "/", "fshape.dat"], dataRef, 0);

	PrintFigure(print);
	if (print == 4)
		gset output "figDRNL800_3.gif";
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
	gset yrange [-20.0:2.0];
	DispPFFuncs([dataRef, "/", "pffunc.dat"], dataRef);

	PrintFigure(print);
	if (print == 4)
		gset output "figDRNL800_4.gif";
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
	gset xtics 200
	#gset yrange [-3.0:2.0];
	#gset xrange [200:1300];
	DispRPFFuncs([dataRef, "/", "rpffunc.dat"], dataRef);

	PrintFigure(print);
	if (print == 4)
		gset output "figDRNL800_5.gif";
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
	gset timestamp bottom;
	gset xtics autofreq;
	gset key bottom;
	DispTTSRatio([dataRef, "/", "ttsfunc.dat"], dataRef);

	PrintFigure(print);
	if (print == 4)
		gset output "figDRNL800_6.gif";
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
	gset xrange [0.0175:0.0225]
	gset xtics 0.001
	DispTTSResp([dataRef, "/", "ttsrfunc.dat"], dataRef);

	PrintFigure(print);
	if (print == 4)
		gset output "figDRNL800_7.gif";
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
	gset xtics autofreq
	gset ytics autofreq
	DispImpResp([dataRef, "/", "irfunc.dat"], 0.001, dataRef);

	PrintFigure(print);
	if (print == 4)
		gset output "figDRNL800_8.gif";
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
		gset output "figDRNL800_9.gif";
	endif

	gset autoscale;
	gset xrange [0:1500]
	DispDProds([dataRef, "/", "dpfunc.dat"], dataRef);

	if (print != 0)
		closeplot;
	endif

endfunction
