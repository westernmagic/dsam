% This function displays the data from the filter investigation program.

function FiltDispGammaC(print)

	dataRef = 'GammaC';

	if (print ~= 0)
		printf('Output sent to printer\n');
	end

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	figure(1);			%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	PrintFigure(print);
	if (print == 4)
	%	gset output 'figGammaC800_1.gif';
	end

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	subplot(2,2,1);
	DispTCurve([dataRef '/' 'tcurve.dat'], dataRef);

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	handle = subplot(2,2,2);
	axis([0.0 1500.0 1.0e5 1.0e7]);
	DispDProds([dataRef '/' 'dpfunc.dat'], dataRef);

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	subplot(2,2,3);
	if (print == 4)
	%	gset xrange [0.0:2100.0];
	end
	DispPFFuncs([dataRef '/' 'pffunc.dat'], dataRef);
	legend_handle = legend;
	set(legend_handle, 'visible', 'off');

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	subplot(2,2,4);
	if (print == 4)
	%	gset yrange [-1500:2500];
	end
	axis([0.030 0.035 -600 600]);
	DispTTSResp([dataRef '/' 'ttsrfunc.dat'], dataRef);

	datestr(now);

%	if (print ~= 0)
%		closeplot;
%	end

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	figure(2);			%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	PrintFigure(print);
	if (print == 4)
	%	gset output 'figGammaC800_2.gif';
	end

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	subplot(2,1,1);
	if (print == 4)
		%gset xrange [0.0:2100.0];
	end
	axis([10 100 -60 100]);
	DispIOFuncs([dataRef '/' 'iofunc.dat'], dataRef, 0);

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	subplot(2,1,2);
%	gset autoscale;
%	gset timestamp bottom;
	if (print == 4)
		%gset xrange [0.0:2100.0];
	end
	DispPIFuncs([dataRef '/' 'pifunc.dat'], dataRef);
%	gset notimestamp;
%	subplot(0, 0);
	
%	if (print ~= 0)
%		closeplot;
%	end

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	figure(3);			%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	PrintFigure(print);
	if (print == 4)
	%	gset output 'figGammaC800_3.gif';
	end

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	subplot(1, 2, 1);
%	gset autoscale;
%	gset timestamp bottom;
	if (print == 4)
		%gset xrange [0.0:2100.0];
	%	gset yrange [10:110];
	%	gset key top;
	else
	%	gset key bottom;
	end
	DispTTSRatio([dataRef '/' 'ttsfunc.dat'], dataRef);
%	gset notimestamp;

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	subplot(1, 2, 2);
%	gset autoscale;
%	gset yrange [1:100];
	if (print == 4)
		%gset xrange [0.0:2100.0];
	end
	DispFShape([dataRef '/' 'fshape.dat'], dataRef, 0);

%	subplot(0, 0);

%	if (print ~= 0)
%		closeplot;
%	end

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	figure(4);			%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%	gset autoscale;
%	gset timestamp bottom;
%	gset ytics autofreq
%	gset xrange [0.02:0.04];
	DispImpResp([dataRef '/' 'irfunc.dat'], 50000, dataRef);
%	gset notimestamp

	PrintFigure(print);
	if (print == 4)
	%	gset output 'figGammaC800_4.gif';
	end
%	if (print ~= 0)
%		replot;
%	end

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	figure(5);			%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	PrintFigure(print);

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	subplot(2, 1, 1);
	axis([200 1300 -70.0 10.0]);
	DispRPFFuncs([dataRef '/' 'rpffunc.dat'], dataRef);

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	subplot(2, 1, 2);
	axis([0.0 1500.0 1e-7 1]);
	DispCGFFuncs([dataRef '/' 'cgffunc.dat'], dataRef);

%	subplot(0, 0);
	
%	if (print ~= 0)
%		closeplot;
%	end

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	figure(6);			%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	PrintFigure(print);

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	subplot(1, 2, 1);
	axis([0.0 1300.0 -15000.0 15000.0]);
	DispCPFFuncs([dataRef '/' 'cpffunc.dat'], dataRef);

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	subplot(1, 2, 2);
	axis([0.0 1300.0 -510.0 510.0]);
	DispRCPFFuncs([dataRef '/' 'rcpffunc.dat'], dataRef);

%	subplot(0, 0);

%	if (print ~= 0)
%		closeplot;
%	end
