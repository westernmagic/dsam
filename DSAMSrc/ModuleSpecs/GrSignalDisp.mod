/* This is a module specifiction file for the DataFile module */
mod_name		SignalDisp;
proc_routine	ShowSignal;
typedef struct {

	/* Signal Controls */
	BOOLN	automaticYScaling;
	int		channelStep;
	double	magnification;
	double	maxY;
	double	minY;
	int		numGreyScales;
	double	width;
	double	xResolution;
	int		yNormalisationMode;

	/* Axis controls */
	char	*yAxisTitle;	/* MAXLINE */
	int		yAxisMode;
	BOOLN	autoYScale;
	char	*yNumberFormat;	/* MAXLINE */
	int		yDecPlaces;
	int		yTicks;
	int		yInsetScale;

	char	*xAxisTitle;	/* MAXLINE */
	char	*xNumberFormat;	/* MAXLINE */
	int		xDecPlaces;
	int		xTicks;
	BOOLN	autoXScale;
	double	xOffset;
	double	xExtent;

	/* General Controls */
	char	*title;			/* [MAXLINE] */
	int		mode;
	double	frameDelay;
	double	topMargin;
	int		frameHeight;
	int		frameWidth;
	int		frameXPos;
	int		frameYPos;
	int		summaryDisplay;
	int		chanActivityDisplay;
	

} SignalDisp;
	
