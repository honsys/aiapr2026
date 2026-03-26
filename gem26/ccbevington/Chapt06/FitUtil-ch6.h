//FitUtil.h

// Move Data
void	PutTitle(char sBuffer[]);
void	PutnPts(int nData);
int		Getm();
void	Putm(int m);
void	PutXYdY(int i, double xin, double yin, double dxin);

void	InvertX();
void	StatisticalErrs();
void	FillYarray();			//fills array Ycalc.  
double	CalcChiSq();		

//Fit procedure
double	LineFit();

// Fitting function
double	PowerSeriesFunc(double x);

//Output
void	FitOut(char outFile[], double chi2, char aErrorsFrom);

