#include <math.h>
#define  PI  3.14159

double Power(double x, double a);					// x^a
double Simpson(char functCode,int nIntervals, double loLim, double hiLim, double p1, double p2, double p3); 
													//2 calcs/interval
double Gamma(double h);								// Approximate Gamma function for integers and half-integers     
double Chi2_Dens(double x, double h, double g);		
double ChiProb(double nFree, double chi2);							// Returns integral from chi2 to infinity. 
double LinCorrel_Dens(double r, double nFree, double dummy);
double LinCorProb(double r, double nFree);							// Returns integral from r to infinity.
double StudentT_Dens(double  t, double nu, double g, double dummy); 
double Stud_tProb(double t, double h);								// Returns integral from -t to +t.
double F_Dens(double f, double nu1, double nu2, double g);			
double F_Prob(double f, double nu1, double nu2);					// Returns integral from f to infinity. 
double GaussProb(double xLim);										// Returns integral from -xLim to + xLim.
