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
	BOOLN	xZoom;
	double	xOffset;
	double	xExtent;

	/* Axis controls */
	char	*yAxisTitle;	/* MAXLINE */
	int		yAxisMode;
	char	*yNumberFormat;	/* MAXLINE */
	int		yDecPlaces;
	int		yTicks;
	int		yInsetScale;

	char	*xAxisTitle;	/* MAXLINE */
	char	*xNumberFormat;	/* MAXLINE */
	int		xDecPlaces;
	int		xTicks;

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
	

} SignalDisp;
	
