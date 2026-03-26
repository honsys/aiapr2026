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

double	Expf(double x, double a);  
double	TwoExponentials(int idummy, double x);

//Added for Chapt_7
double	MatInv();							//invert a matrix
void	SquareByRow();		// multiply square matrix by linear matrix
void	SigMatrix();     

// added for Chapt_8
void	ReadParameterFile_NonLin(char fName[], double stepSize);
void	Gridls(double &chiSqr);
void	Gradls(double &chiSqr, double stepDown);
void	ChiFit(double &chiSqr);
void	Marquardt(double &chiSqr, double chiCut, double lambda);
void	SigParab();
void	CalcGrad();
double	dXiSq_da(int j);
double	d2XiSq_da2(int j);
double	d2XiSq_dajk(int j, int k);


//Output

	
void	FitOut(char outFile[], double chi2, char aErrorsFrom);

