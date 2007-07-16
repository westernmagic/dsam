# This function displays the data from the filter investigation program.

function FiltDisp1

	figure(1);
	multiplot(2,2);
	clearplot;

	subwindow(1, 1);
	gset autoscale;
	DispTCurve("tcurve.dat");

	subwindow(2, 1);
	gset autoscale;
	DispFShape("fshape.dat");

	subwindow(1, 2);
	gset autoscale;
	gset xtics 500
	DispPFFuncs("pffunc.dat");
	gset xtics autofreq

	subwindow(2, 2);
	gset autoscale;
	DispTTSRatio("ttsfunc.dat");

	figure(2);
	multiplot(1,2);
	clearplot;
	subwindow(1, 1);
	gset autoscale;
	DispIOFuncs("iofunc.dat");

	subwindow(1, 2);
	gset autoscale;
	DispPIFuncs("pifunc.dat");
	
	figure(3);
	clearplot;
	multiplot(2, 1);
	subwindow(1, 1);
	gset autoscale;
	gset xrange [0.0175:0.0225]
	gset xtics 0.001
	DispTTSResp('ttsrfunc.dat');
	gset xtics autofreq

	subwindow(2, 1);
	gset autoscale;
	gset xrange [0:20000]
	DispDProds('dpfunc.dat');

	figure(4);
	clearplot;
	gset autoscale;
	DispImpResp('irfunc.dat', 0.02);

	multiplot(0, 0);

endfunction
