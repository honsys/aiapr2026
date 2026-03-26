#include <iomanip.h>
#include <math.h>
#define  PI  3.14159

double Power(double x, double a);					// x^a
double Simpson(char functCode,int nIntervals, double loLim, double hiLim, double p1, double p2, double p3); 
													//2 calcs/interval
double Gamma(double h);								// Approximate Gamma function for integers and half-integers     
double Chi2_Dens(double x, double h, double g);		
double ChiProb( double nFree, double chi2);							// Returns integral from chi2 to infinity. 
double LinCorrel_Dens(double r, double nFree, double dummy);
double LinCorProb(double r, double nFree);							// Returns integral from r to infinity.
double StudentT_Dens(double  t, double nu, double g, double dummy); 
double Stud_tProb(double t, double h);								// Returns integral from -t to +t.
double F_Dens(double f, double nu1, double nu2, double g);			
double F_Prob(double f, double nu1, double nu2);					// Returns integral from f to infinity. 
double GaussProb(double xLim);										// Returns integral from -xLim to + xLim.


// --------------------------------- Calculate X^a -------------------------------

double Power(double x, double a)	//x^a
{
	double tem;

	tem = exp(a*log(x));
	return(tem);
}

//-------------------------------- Gamma Function ---------------------------- 

double Gamma(double h)  //Approximate Gamma function for integers and half-integers     
{
	double gamma;		//gamma  = sqrt(2*PI)*[1 + 0.0833/h]*exp(-h)*[h^(h-1/2)] }
	gamma = sqrt(2.* PI)*(1 + 0.0833/h)*exp(-h)*Power(h, h-0.5);
	return(gamma);
}


// ------------------------------------------------- Gaussian -----------------------------------------------
double Gauss_Dens(double x, double dummy1, double dummy2, double dummy3)						// Standard Gaussian with mean = 0 and st.dev = 1
{
   return(exp(-x*x/2)/sqrt(2*PI));
}

double GaussProb(double xLim)	// Returns integral from -xLim to + xLim.
{
	double y;
	y = Simpson('G', 20, -xLim, xLim, 0, 0, 0);
	return(y);
}

			
//-------------------------------- Chi^2 Density and Probability ---------------------------- 

double Chi2_Dens(double x, double h, double g, double dummy1) //See ChiProb for g.
{
	double chiDens;
	
	if (x == 0.) 
		chiDens = 0.; 
	else
		chiDens = Power(x, h-1)*exp(-x/2.)/g;		//  x^(h-1) e^(-x/2);
	return(chiDens);
}

double	ChiProb( double nFree, double chi2)
{
	double	probTem,  
			cLim = 2,								// expansion limit for nFree = 1
			intFromLim = 0.157,						// integral from cLim to infInity for nFree = 1
			dx = 0.2,
			g; 								// determines accuracy of integration}
	int nIntervals = chi2/dx;		// number  of intervals for Simpson integral.  2 calculations per interval.
	double h = (double) nFree/2.;

	g = Gamma(h) * Power( 2, h);
	if (chi2 > 15*sqrt(nFree))  
	return(0.0);	// quick cutout  	
	if (nFree == 1)
	{
		if (chi2 < cLim)						// Integrate expansion of the function 
			probTem = 1.0-sqrt(chi2/2./PI)*(2. - chi2*(1./3. - chi2*(1.0/20. - chi2*(1./168. - chi2/1728.))));
		else
			// Subtract numerical integral from analytic intFromLim 
			probTem = intFromLim - Simpson('X',nIntervals,cLim,chi2,h, g, 0.);
		return(probTem);
	}
		if (nFree == 2) 
			probTem = exp(-chi2/2);				// Integrable for nFree = 2;
		else									// I.e., nFree > 2
			probTem = 1 - Simpson('X', nIntervals, 0, chi2, h, g, 0.);
	return(probTem);
}
			;
// ------------------------------ Linear-correlation Function and Integral -----------------------------------------

//Program 11.3: Linear-correlation probability function & integral

double LinCorrel_Dens(double r, double nFree, double dummy, double dummy1)

{	double tem;
	tem = Gamma((nFree+1.)/2.)/Gamma(nFree/2.) * exp((nFree-2.)/2. * log(1. - r*r))/sqrt(PI);
	return(tem);
}
									
double LinCorProb(double r, double nFree)
{
	const	double		dx		= 0.01;
	double	h, tem;
	int		nIntervals;

	h = nFree;
	nIntervals   = r/dx;
	tem   = 1.-2.*Simpson('C', nIntervals, 0.,  r, h, 0,0.);
	return(tem);
};


//-------------------------------------------- F Distribution and Integral Probability ------------------

double F_Dens(double f, double nu1, double nu2, double g)	// f Probability density. See F_Prob for g. 
{
	double tem;			//f = chi1*nu2/(chi2*nu1)
	if (f==0)
		tem =0;
	else
	{
		tem = g* Power(f,0.5*(nu1-2.))/Power( 1.+ f*nu1/nu2, 0.5*(nu1+nu2)); 
	}
	return(tem);
}


double 	F_Prob(double f, double nu1, double nu2)  //returns integral from f to infinity
{	
	double tem, g;
	int nIntervals   = 1000;

	g = Gamma(0.5*(nu1+nu2))/(Gamma(0.5*nu1)*Gamma(0.5*nu2))*Power(nu1/nu2,0.5*nu1);
	tem   = 1.0 - Simpson('F', nIntervals, 0,  f, nu1, nu2, g);
	return(tem);
}

// ---------------------------------Integral Probability for Student's t ----------------------

double StudentT_Dens(double  t, double nu, double g, double dummy) //Student's t function. See Stud_Prob for g.
{
	double  x;

	x = g*exp(  (-0.5*(nu+1.)) * log(1+t*t/nu));
	return (x);
}

// ---------------------------- Students t Probability
double 	Stud_tProb(double t, double h)  //returns integral from -t to +t
{	
	double g,  dt, tem;
	int nIntervals;

	g  = (Gamma((h+1)/2)/Gamma(h/2))/sqrt(h*PI);		//indep of t - calculate once
	dt		= 0.01;										//integration step
	nIntervals   = t/dt;
	tem   = 2.*Simpson('T', nIntervals, 0,  t, h, g, 0.);
	return(tem);
}

//------------------------------------------------------------------------------------------------------

//----Simpson's rule  args: funct selects the function, ... p1, and p2 are parameters of the function.

double Simpson(char functCode,int nIntervals, double loLim, double hiLim, double p1, double p2, double p3)   //2 calcs/interval
{
	double (*SimpsonFunct)(double, double, double, double);
	double sum, x,dx,      t1,t2;
	int i;

	x = loLim;
	dx = (hiLim - loLim)/(2*nIntervals);
// --------------- Select function- must be in this list
	switch (functCode)
	{
		case 'X':
			SimpsonFunct = Chi2_Dens;
		break;
		case 'C':
			SimpsonFunct = LinCorrel_Dens;
		break;
		case 'T':
			SimpsonFunct = StudentT_Dens;
		break;
		case 'F':
		SimpsonFunct = F_Dens;
		break;
		case 'G':
		SimpsonFunct = Gauss_Dens;
		break;
	}
	sum = SimpsonFunct(loLim, p1, p2,p3) - SimpsonFunct(hiLim, p1, p2,p3);
	for (i=1; i <= nIntervals; i++) 
	{
		x = x + 2*dx;

		t1 = SimpsonFunct(x-dx, p1, p2, p3);
		t2 = SimpsonFunct(x,    p1 ,p2, p3);
		sum = sum + 4*t1 + 2*t2;
//		cout << x << "   " << t2 << "  " << sum <<  endl;
		x = x;
	}
	sum = sum*dx/3;
	return(sum);
}



