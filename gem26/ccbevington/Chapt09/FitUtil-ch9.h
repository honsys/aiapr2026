//c:\cpp\CppLibrary\FitUtil.h

// Move Data
void	PutTitle(char sBuffer[]);
void	PutnPts(int nData);
int		Getm();
void	Putm(int m);
double	GetParam(int i);
void	PutParam(int i, double aNew);
void	PutParamIncr(int i, double deltaaNew);

void	PutXYdY(int i, double xin, double yin, double dxin);

void	SetLinear(bool trueOrfalse);		// Linear or non-linear fitting

void	ChooseFunction(char funcCode, char code1Set);
void	InvertX();
void	StatisticalErrs();
void	FillYarray();						//fills array Ycalc.  
double	CalcChiSq();		
double	LineFit();

double	PowerSeriesFunc(int k, double x);
double	LegendreFunc(int k, double x);
double	Expf(double x, double a);  
double	TwoExponentials(int idummy, double x);

//Added for Chapt_7
void	MakeBeta();
void	MakeAlpha();
double	MatInv();							//invert a matrix
void	SquareByRow();		// multiply square matrix by linear matrix
void	SigMatrix();     

// added for Chapt_8
void	ReadParameterFile_NonLin(char fName[], double stepSize);
void	Marquardt(double &chiSqr, double chiCut, double lambda);
void	SigParab();
double	dXiSq_da(int j);
double	d2XiSq_da2(int j);
double	d2XiSq_dajk(int j, int k);

// added for Chapter_9
double	OneLorentz(int idummy, double x);				// Calculate one peak on background
double	TwoLorentz(int idummy, double x);				// Calculate two peaks on background
double	Lorentz(double x, double mean, double gamma);	// Calculate the peak

//Input Output

void	FitOut(char outFile[], double chi2, char aErrorsFrom);

