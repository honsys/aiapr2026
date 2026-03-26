//FitUtil-ch7.h

// Move Data
void	PutTitle(char sBuffer[]);
void	PutnPts(int nData);
int		Getm();
void	Putm(int m);
double	GetParam(int i);
void	PutParam(int i, double aNew);

void	PutXYdY(int i, double xin, double yin, double dxin);

void	ChooseFunction(char funcCode, char code1Set);
void	InvertX();
void	StatisticalErrs();
void	FillYarray();						//fills array Ycalc.  
double	CalcChiSq();		
double	LineFit();

double	PowerSeriesFunc(int k, double x);
double	LegendreFunc(int k, double x);

//Added for Chapt_7
void	MakeBeta();
void	MakeAlpha();
double	MatInv();							//invert a matrix
void	SquareByRow();		// multiply square matrix by linear matrix
void	SigMatrix();     

//Output
	
void	FitOut(char outFile[], double chi2, char aErrorsFrom);

