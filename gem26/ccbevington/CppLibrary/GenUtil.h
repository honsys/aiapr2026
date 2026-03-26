
// GenUtil.h;

#define PI 3.14159

//File_IO  Input-Output	

void	OpenInFile(char *Fname, int length);
void	ReadString(char *sBuffer, int length);
void	ReadIntegers(int numNumbers,int *Buffer,int iLength) ;
void	ReadDoubles(int numNumbers,double *Buffer,double dLength);

double	sqr(double x);	
void	ErrorMessage(const char message[]);
char	pause();				//--- pause routine for ch 6

//double	Simpson(char functCode,int nInt, double loLim, double hiLim, double p1, double p2); // 2 calcs per interval
//double	ChiX(double x, double h, double dummy);			//Data for Simpson,. h = nFree/2
double	ChiProb( double nFree, double chi2);
double	Chi2_Dens(double x, double h, double g); //See ChiProb for g.
double	Simpson(char functCode,int nIntervals, double loLim, double hiLim, double p1, double p2, double p3);   //2 calcs/interval

double	Gamma(double h);					//Approximate Gamma function for integers and half-integers




