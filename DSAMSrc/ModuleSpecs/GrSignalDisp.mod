/* This is a module specifiction file for the DataFile module */
mod_name		SignalDisp;
proc_routine	ShowSignal;
typedef struct {

	char	*title;			/* [MAXLINE] */
	int		automaticYScaling;
	int		channelStep;
	int		frameHeight;
	int		frameWidth;
	int		frameXPos;
	int		frameYPos;
	int		yAxisMode;
	int		yNormalisationMode;
	int		mode;
	int		numGreyScales;
	int		summaryDisplay;
	int		xTicks;
	int		xDecPlaces;
	int		yDecPlaces;
	int		yTicks;
	int		yInsetScale;
	
	double	frameDelay;
	double	magnification;
	double	xResolution;
	double	minY;
	double	maxY;
	double	topMargin;
	double	width;

} SignalDisp;
	
