// Program 7.1: Least-squares fit by matrices to a power series and to Legendre polynomials.
//Program MaxLikelihood;
#include <fstream.h>
#include <math.h>
#include "c:\cpp\CppLibrary\GenUtil.h"


int		i, j, nEvents, nRead;
//-----------------------------------------------------------------------

double	mass, d1, d2, LtoTscale;
//------------------------------Data Arrays for Search ------------------
const double	c			= 3.0;		//vel. of light
const int		maxEvents	= 3200;

double loTlim[maxEvents], hiTlim[maxEvents], times[maxEvents]; 

// --------------- Search range and step-------------------------------
double const	loSearch  = 0.2 ;
double const	hiSearch  = 1.2 ;
double const	tauStep   = 0.01;
int	   const	nTrials		=  floor((hiSearch - loSearch)/tauStep);

//------------------------------------------------------------------------
double LogProb(int k, double tau)
{
	double   a, b ;
	double   prob ; //debug
																
	// ------Now, calc probability------						
	b = exp(-hiTlim[k]/tau);
	a = exp(-loTlim[k]/tau);
	prob = exp(-times[k]/tau)/(tau*(a - b));
	return(log(prob));	//LogProb  = -times[k]/tau - ln(tau*(a - b));  //log likelihood					
}

//------------------------------------------------------------------------
double LogLike(double t)
{
	int  i;     
	double M, prob;

	M = 0.0;
	for (i = 1; i <= nEvents; i++)
	{
		prob = LogProb(i,t);
		M = prob + M;
	};
	return(M);
}


//------------------------------------------------------------------------

void Search(double &tauAtMax, double &maxM)
{
	int trial; 
	double M1, M2, M3, del1, del2, delta1, tau, MLikeLi;
	M2    =  -1000;
	maxM  =  -1.0e20;
	tau   =   loSearch;
	for (trial = 0; 0<= nTrials; trial++)
	{
		MLikeLi = LogLike(tau);
		cout <<"trial " << trial 
			 <<" tau= " << tau 
			<<" Log Likelihood= " 
			<<  MLikeLi
			<< " M2= "
			<< M2
			<<  endl;
		M3 = MLikeLi;
		if ((M3 > M2) || (trial < 3))          //Remember, these are negative
		{										//Still heading to maximum
			M1 = M2;
			M2 = M3;
		} 
		else
		{    //leaving maximum. Find maximum of parabola defined by last three points-  
			del1     = M2 - M1;
			del2     = M3 - 2*M2 + M1;
			delta1   = tauStep * (del1/del2 + 1.5);
			tau      = tau  - delta1;
			tauAtMax = tau;
			maxM     = LogLike(tau);    //at  maximum of parabola
			cout <<" Max from parabola:  tau= " << tau 
				 <<" Log Likelihood= "<< maxM
				 << endl;
			return;
		}	
		tau = tau + tauStep;
	}	
}

// ---------------------------------- Read Parameter and Data Files ----------------------------------------

//--------------------  Read Buffers -----------------------------------
char	sBuffer[] = "                                                     ";
int		iiBuffer[30];
double	ddBuffer[30];
int nInts, nData, nDbls; 

void ReadParam()
{
	ReadString(sBuffer, sizeof(sBuffer)/sizeof(char));
	nInts = 1;
	ReadIntegers(nInts, iiBuffer, sizeof(iiBuffer)/sizeof(int));
	nData = iiBuffer[1];
	nDbls = 3;
	for (j = 1;j<= nData; j++)
	{
		ReadDoubles(nDbls, ddBuffer, sizeof(ddBuffer)/sizeof(double));
	}		
	mass	=	ddBuffer[1];
	d1		=	ddBuffer[2]; 
	d2		=	ddBuffer[3]; 
}

void ReadData()
{
	double	evNum, xProduction, pLab, xDecay;

	ReadString(sBuffer, sizeof(sBuffer)/sizeof(char));
	ReadIntegers(nInts, iiBuffer, sizeof(iiBuffer)/sizeof(int));
	nData	= iiBuffer[1];			// number of events
	nDbls	= 5;						// number of variables per event (i.e., on a line)
	nRead	= 0;
	nEvents = 0;

	do		
	{
		ReadDoubles(nDbls, ddBuffer, sizeof(ddBuffer)/sizeof(double));
		evNum		= ddBuffer[1]; 
		xProduction	= ddBuffer[2];
		pLab		= ddBuffer[3];
		xDecay		= ddBuffer[4];

		cout  <<evNum << "  " << xProduction <<"  " << pLab << "  "<< xDecay;
		nRead++;
		if ((xDecay >= d1) && (xDecay < d2))
		{
			nEvents++;															// within cuts
			LtoTscale =  mass/(c*pLab);											// this is 1/(c*beta*gamma) 
			times[nEvents] = (xDecay - xProduction)*LtoTscale;					// proper time
		//-----------Must cvt d1q and d2 to loTlim and hiTlim which are integration limits in proper time
		//           measured from prod vertex.
			loTlim[nEvents] = (d1-xProduction)  * LtoTscale;
			hiTlim[nEvents] = (d2 -xProduction) * LtoTscale;
		} 
		 cout <<"  "<<nEvents <<endl;
	}
	while (ddBuffer[1] >- 0); //while (nEvents < 200); 
	nEvents--;
	cout <<  "end of File " << nRead <<" events read, "<< nEvents <<" passed cuts" <<endl;
}

// ----------------------------------------------------------------------
double Error(double t, double dt)  //1/sqrt[-2nd derivative of log(L)]
{
	double t1, t2, d2Ydt2, err ;

	t1		= t - dt;
	t2		= t + dt;
	d2Ydt2	= (LogLike(t2) - 2*LogLike(t) + LogLike(t1))/sqr(dt);
  err		= 1/sqrt(-d2Ydt2);
  return(err);
}

// ---------------------------------------------------------------------------------------
void OutPut(double tauAtMax, double maxM)
{
double sgTau;
	sgTau = Error(tauAtMax, tauStep);
	cout << endl;
	cout <<"N surviving events " << nEvents 
		<< " max log likelihood= " << maxM << endl 
        <<"tau at max = "<< tauAtMax<<"+-" << sgTau << endl;
	cout << endl;
	pause();
}
// --------------------------------------------- MAIN --------------------------------------

void main()
{
	char FileName1[] = "MaxLike.par";
	char FileName2[] = "MaxLike.dat";
	cout << sBuffer << endl;
	double  tauMax, maxM ;    //M is log of likelihood function

	OpenInFile(FileName1, sizeof(FileName1)/sizeof(char));			
	ReadParam();									// Read file "ThermCouple-E7-2.Dat"
	OpenInFile(FileName2, sizeof(FileName2)/sizeof(char));			
	ReadData();									// Read file "ThermCouple-E7-2.Dat"
	Search(tauMax, maxM);
	OutPut(tauMax, maxM);
 }	
