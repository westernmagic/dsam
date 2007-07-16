# This function displays the data from the filter investigation program.

function FiltDispFIR1(print)

	dataRef = "MiddleEar";

	if (print != 0)
		printf("Output sent to printer\n");
	endif

	############################################################################
	figure(1);			########################################################
	############################################################################
	PrintFigure(print);
	multiplot(2,2);

	############################################################################
	subwindow(1, 1);
	clearplot;
	gset autoscale;
	gset ytics 10
	DispTCurve([dataRef, "/", "tcurve.dat"], dataRef);
	gset ytics autofreq

	############################################################################
	subwindow(2, 1);
	clearplot;
	gset autoscale;
	gset xrange [0:1500]
	DispDProds([dataRef, "/", "dpfunc.dat"], dataRef);

	############################################################################
	subwindow(1, 2);
	clearplot;
	gset autoscale;
	gset xtics 500
	gset timestamp bottom;
	DispPFFuncs([dataRef, "/", "pffunc.dat"], dataRef);
	gset notimestamp;
	gset xtics autofreq

	############################################################################
	subwindow(2, 2);
	clearplot;
	gset autoscale;
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
	multiplot(1,2);

	############################################################################
	subwindow(1, 1);
	clearplot;
	gset autoscale;
	DispIOFuncs([dataRef, "/", "iofunc.dat"], dataRef, 1);

	############################################################################
	subwindow(1, 2);
	clearplot;
	gset autoscale;
	gset timestamp bottom;
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
	multiplot(2, 1);

	############################################################################
	subwindow(1, 1);
	clearplot;
	gset autoscale;
	gset timestamp bottom;
	DispTTSRatio([dataRef, "/", "ttsfunc.dat"], dataRef);
	gset notimestamp;

	subwindow(2, 1);
	clearplot;
	gset autoscale;
	DispFShape([dataRef, "/", "fshape.dat"], dataRef, 1);

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
	DispImpResp([dataRef, "/", "irfunc.dat"], 0.001, dataRef);
	gset notimestamp

	PrintFigure(print);
	if (print != 0)
		replot;
	endif

	if (print != 0)
		closeplot;
	endif

endfunction
