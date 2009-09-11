/* This is a module specifiction file for the DataFile module */
mod_name		SignalDisp;
proc_routine	ShowSignal;
typedef struct {

	/* Signal Controls */
	BOOLN	automaticYScaling;
	int		channelStep;
	Float	magnification;
	Float	maxY;
	Float	minY;
	int		numGreyScales;
	Float	width;
	Float	xResolution;
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
	Float	xOffset;
	Float	xExtent;

	/* General Controls */
	char	*title;			/* [MAXLINE] */
	int		mode;
	Float	frameDelay;
	Float	topMargin;
	int		frameHeight;
	int		frameWidth;
	int		frameXPos;
	int		frameYPos;
	int		summaryDisplay;
	int		chanActivityDisplay;
	

} SignalDisp;
	
