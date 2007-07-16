# This function displays the data from the filter investigation program.

function FiltDispCarney800(print)

	dataRef = "Carney800";

	if (print != 0)
		printf("Output sent to printer\n");
	endif

	############################################################################
	figure(1);			########################################################
	############################################################################
	PrintFigure(print);
	if (print == 4)
		gset output "figCarney800_1.gif";
	endif
	multiplot(2,2);

	############################################################################
	subwindow(1, 1);
	clearplot;
	gset autoscale;
	gset ytics 10
	gset xtics autofreq
	DispTCurve([dataRef, "/", "tcurve.dat"], dataRef);

	############################################################################
	subwindow(2, 1);
	clearplot;
	gset autoscale;
	gset xrange [0:1500]
	gset yrange [1.0e4:1.0e7]
	DispDProds([dataRef, "/", "dpfunc.dat"], dataRef);

	############################################################################
	subwindow(1, 2);
	clearplot;
	gset autoscale;
	gset xtics 500
	gset timestamp bottom;
	gset yrange [-20.0:2.0];
	if (print == 4)
		gset xrange [0.0:2100.0];
	endif
	DispPFFuncs([dataRef, "/", "pffunc.dat"], dataRef);
	gset notimestamp;
	gset xtics autofreq

	############################################################################
	subwindow(2, 2);
	clearplot;
	gset autoscale;
	gset ytics autofreq
	if (print == 4)
		gset yrange [-1500:2500];
	endif
	gset xrange [0.0175:0.0225]
	gset xtics 0.001
	DispTTSResp([dataRef, "/", "ttsrfunc.dat"], dataRef);
	gset xtics autofreq
	multiplot(0, 0);

	if (print != 0)
		closeplot;
	endif

	############################################################################
	figure(2);			########################################################
	############################################################################
	PrintFigure(print);
	if (print == 4)
		gset output "figCarney800_2.gif";
	endif
	multiplot(1,2);

	############################################################################
	subwindow(1, 1);
	clearplot;
	gset autoscale;
	gset ytics autofreq
	if (print == 4)
		#gset xrange [0.0:2100.0];
		gset yrange [0.1:1e6];
	endif
	DispIOFuncs([dataRef, "/", "iofunc.dat"], dataRef, 1);

	############################################################################
	subwindow(1, 2);
	clearplot;
	gset autoscale;
	gset timestamp bottom;
	if (print == 4)
		#gset xrange [0.0:2100.0];
		gset yrange [-0.4:1.2];
	endif
	DispPIFuncs([dataRef, "/", "pifunc.dat"], dataRef);
	gset notimestamp;
	multiplot(0, 0);
	
	if (print != 0)
		closeplot;
	endif

	############################################################################
	figure(3);			########################################################
	############################################################################
	PrintFigure(print);
	if (print == 4)
		gset output "figCarney800_3.gif";
	endif
	multiplot(2, 1);

	############################################################################
	subwindow(1, 1);
	clearplot;
	gset autoscale;
	gset timestamp bottom;
	if (print == 4)
		#gset xrange [0.0:2100.0];
		gset yrange [10:110];
		gset key top;
	else
		gset key bottom;
	endif
	DispTTSRatio([dataRef, "/", "ttsfunc.dat"], dataRef);
	gset notimestamp;

	############################################################################
	subwindow(2, 1);
	clearplot;
	gset autoscale;
	if (print == 4)
		#gset xrange [0.0:2100.0];
		gset yrange [0.01:1e6];
	endif
	DispFShape([dataRef, "/", "fshape.dat"], dataRef, 0);

	multiplot(0, 0);

	if (print != 0)
		closeplot;
	endif

	############################################################################
	figure(4);			########################################################
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
		gset output "figCarney800_4.gif";
	endif
	if (print != 0)
		replot;
	endif

	############################################################################
	figure(5);			########################################################
	############################################################################
	PrintFigure(print);
	if (print == 4)
		gset output "figCarney800_5.gif";
	endif
	multiplot(1, 2);

	############################################################################
	subwindow(1, 1);
	clearplot;
	gset autoscale;
	#gset xtics 5000
	gset yrange [-3.0:2.0];
	gset xrange [200:1300];
	DispRPFFuncs([dataRef, "/", "rpffunc.dat"], dataRef);
	gset ytics autofreq

	############################################################################
	subwindow(1, 2);
	clearplot;
	gset autoscale;
	gset xtics autofreq
	gset timestamp bottom;
	gset yrange [0.01:0.1]
	gset xrange [0.0:2000]
	DispCGFFuncs([dataRef, "/", "cgffunc.dat"], dataRef);
	gset notimestamp;
	gset xtics autofreq

	multiplot(0, 0);
	
	if (print != 0)
		closeplot;
	endif

	############################################################################
	figure(6);			########################################################
	############################################################################
	PrintFigure(print);
	if (print == 4)
		gset output "figCarney800_6.gif";
	endif
	multiplot(2, 1);

	############################################################################
	subwindow(1, 1);
	clearplot;
	gset autoscale;
	gset timestamp bottom;
	gset xrange [0:1300]
	DispCPFFuncs([dataRef, "/", "cpffunc.dat"], dataRef);
	gset notimestamp;

	############################################################################
	subwindow(2, 1);
	clearplot;
	gset autoscale;
	gset ytics autofreq
	gset xtics autofreq
	#gset yrange [-110:110]
	gset xrange [0:1300]
	DispRCPFFuncs([dataRef, "/", "rcpffunc.dat"], dataRef);

	multiplot(0, 0);

	if (print != 0)
		closeplot;
	endif

endfunction
