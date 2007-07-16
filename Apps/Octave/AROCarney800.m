# This function displays the data from the filter investigation program.

function AROCarney800(print)

	dataRef = "Carney800";

	if (print != 0)
		printf("Output sent to printer\n");
	endif

	############################################################################
	figure(1);			########################################################
	############################################################################
	clearplot;
	PrintFigure(print);
	if (print == 4)
		gset output "figCarney800_1.gif";
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
	gset ytics autofreq
	if (print == 4)
		#gset xrange [0.0:2100.0];
		gset yrange [0.1:1e6];
	endif
	DispIOFuncs([dataRef, "/", "iofunc.dat"], dataRef, 1);

	PrintFigure(print);
	if (print == 4)
		gset output "figCarney800_2.gif";
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
	if (print == 4)
		#gset xrange [0.0:2100.0];
		#gset yrange [0.01:1e6];
	endif
	DispFShape([dataRef, "/", "fshape.dat"], dataRef, 0);

	PrintFigure(print);
	if (print == 4)
		gset output "figCarney800_3.gif";
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
	if (print == 4)
		gset xrange [0.0:2100.0];
	endif
	DispPFFuncs([dataRef, "/", "pffunc.dat"], dataRef);

	PrintFigure(print);
	if (print == 4)
		gset output "figCarney800_4.gif";
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
	gset yrange [-3.0:2.0];
	gset xrange [200:1300];
	DispRPFFuncs([dataRef, "/", "rpffunc.dat"], dataRef);

	PrintFigure(print);
	if (print == 4)
		gset output "figCarney800_5.gif";
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
	if (print == 4)
		#gset xrange [0.0:2100.0];
		gset yrange [10:110];
		gset key bottom;
	else
		gset xtics autofreq;
	endif
	DispTTSRatio([dataRef, "/", "ttsfunc.dat"], dataRef);

	PrintFigure(print);
	if (print == 4)
		gset output "figCarney800_6.gif";
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
	if (print == 4)
		gset yrange [-1500:2500];
	endif
	gset xrange [0.0175:0.0225]
	gset xtics 0.001
	DispTTSResp([dataRef, "/", "ttsrfunc.dat"], dataRef);

	PrintFigure(print);
	if (print == 4)
		gset output "figCarney800_7.gif";
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
	gset ytics autofreq
	gset xtics autofreq
	DispImpResp([dataRef, "/", "irfunc.dat"], 1000, dataRef);
	gset notimestamp

	PrintFigure(print);
	if (print == 4)
		gset output "figCarney800_8.gif";
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
		gset output "figCarney800_9.gif";
	endif

	gset autoscale;
	gset xrange [0:1500]
	gset yrange [1.0e4:1.0e7]
	DispDProds([dataRef, "/", "dpfunc.dat"], dataRef);

	if (print != 0)
		replot;
		closeplot;
	endif

endfunction
