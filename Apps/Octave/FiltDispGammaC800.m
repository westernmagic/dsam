# This function displays the data from the filter investigation program.

function FiltDispGammaC800(print)

	dataRef = 'GammaC800';

	if (print ~= 0)
		printf('Output sent to printer\n');
	end

	###########################################################################
	figure(1);			#######################################################
	###########################################################################
	PrintFigure(print);
	if (print == 4)
		gset output 'figGammaC800_1.gif';
	end
	multiplot(2,2);

	###########################################################################
	subwindow(1, 1);
	clearplot;
	gset autoscale;
	gset ytics 10
	gset xtics autofreq
	DispTCurve([dataRef '/' 'tcurve.dat'], dataRef);

	###########################################################################
	subwindow(2, 1);
	clearplot;
	gset autoscale;
	gset xtics 400;
	gset xrange [0:1500]
	gset yrange [1.0e5:1.0e7]
	DispDProds([dataRef '/' 'dpfunc.dat'], dataRef);

	###########################################################################
	subwindow(1, 2);
	clearplot;
	gset autoscale;
	gset xtics 500
	gset timestamp bottom;
	gset ytics autofreq;
	if (print == 4)
		gset xrange [0.0:2100.0];
	end
	DispPFFuncs([dataRef '/' 'pffunc.dat'], dataRef);
	gset notimestamp;
	gset xtics autofreq

	###########################################################################
	subwindow(2, 2);
	clearplot;
	gset autoscale;
	gset ytics autofreq
	if (print == 4)
		gset yrange [-1500:2500];
	end
	gset xrange [0.030:0.035]
	gset xtics 0.001
	DispTTSResp([dataRef '/' 'ttsrfunc.dat'], dataRef);
	gset xtics autofreq
	multiplot(0, 0);

	if (print ~= 0)
		closeplot;
	end

	###########################################################################
	figure(2);			#######################################################
	###########################################################################
	PrintFigure(print);
	if (print == 4)
		gset output 'figGammaC800_2.gif';
	end
	multiplot(1,2);

	###########################################################################
	subwindow(1, 1);
	clearplot;
	gset autoscale;
	gset ytics autofreq
	%gset yrange [1:100];
	gset key outside;
	if (print == 4)
		gset xrange [0.0:2100.0];
	end
	DispIOFuncs([dataRef '/' 'iofunc.dat'], dataRef, 0);

	###########################################################################
	subwindow(1, 2);
	clearplot;
	gset autoscale;
	gset timestamp bottom;
	if (print == 4)
		gset xrange [0.0:2100.0];
	end
	DispPIFuncs([dataRef '/' 'pifunc.dat'], dataRef);
	gset notimestamp;
	multiplot(0, 0);
	
	if (print ~= 0)
		closeplot;
	end

	###########################################################################
	figure(3);			#######################################################
	###########################################################################
	PrintFigure(print);
	if (print == 4)
		gset output 'figGammaC800_3.gif';
	end
	multiplot(2, 1);

	###########################################################################
	subwindow(1, 1);
	clearplot;
	gset autoscale;
	gset timestamp bottom;
	if (print == 4)
		gset xrange [0.0:2100.0];
		gset yrange [10:110];
		gset key top;
	else
		gset key bottom;
	end
	DispTTSRatio([dataRef '/' 'ttsfunc.dat'], dataRef);
	gset notimestamp;

	###########################################################################
	subwindow(2, 1);
	clearplot;
	gset autoscale;
	#gset yrange [1:100];
	if (print == 4)
		gset xrange [0.0:2100.0];
	end
	DispFShape([dataRef '/' 'fshape.dat'], dataRef, 0);

	multiplot(0, 0);

	if (print ~= 0)
		closeplot;
	end

	###########################################################################
	figure(4);			#######################################################
	###########################################################################
	clearplot;
	gset autoscale;
	gset timestamp bottom;
	gset ytics autofreq
	gset xrange [0.02:0.04];
	DispImpResp([dataRef '/' 'irfunc.dat'], 50000, dataRef);
	gset notimestamp

	PrintFigure(print);
	if (print == 4)
		gset output 'figGammaC800_4.gif';
	end
	if (print ~= 0)
		replot;
	end

	###########################################################################
	figure(5);			#######################################################
	###########################################################################
	PrintFigure(print);
	multiplot(1, 2);

	###########################################################################
	subwindow(1, 1);
	clearplot;
	gset autoscale;
	gset xtics 5000
	#gset yrange [-3.0:2.0];
	gset xrange [200:1300];
	DispRPFFuncs([dataRef '/' 'rpffunc.dat'], dataRef);
	gset ytics autofreq

	###########################################################################
	subwindow(1, 2);
	clearplot;
	gset autoscale;
	gset xtics autofreq
	gset timestamp bottom;
	#gset yrange [0.01:0.1]
	gset xrange [0.0:1500]
	DispCGFFuncs([dataRef '/' 'cgffunc.dat'], dataRef);
	gset notimestamp;
	gset xtics autofreq

	multiplot(0, 0);
	
	if (print ~= 0)
		closeplot;
	end

	###########################################################################
	figure(6);			#######################################################
	###########################################################################
	PrintFigure(print);
	multiplot(2, 1);

	###########################################################################
	subwindow(1, 1);
	clearplot;
	gset autoscale;
	gset timestamp bottom;
	gset xrange [0:1300]
	DispCPFFuncs([dataRef '/' 'cpffunc.dat'], dataRef);
	gset notimestamp;

	###########################################################################
	subwindow(2, 1);
	clearplot;
	gset autoscale;
	gset ytics autofreq
	gset xtics autofreq
	gset yrange [-510:510]
	gset xrange [0:1300]
	DispRCPFFuncs([dataRef '/' 'rcpffunc.dat'], dataRef);

	multiplot(0, 0);

	if (print ~= 0)
		closeplot;
	end


