//  c:\cpp\Cpp\Library\FitUtil  -  Utilities for Least Squares Fitting


#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;
#include <math.h>
#include <string.h>

#include "GenUtil.h"
#include "FitUtil.h"
//================================= Fitting Variables ==============================================

//---------------------------- Data and Parameter Arrays ----------------------------------
const	int nParam = 10;		// max number of parameters
const	int nData = 10000;		// max number of data coordinates
double	x[nData], y[nData], sigY[nData], yCalc[nData], y_0[nData];	// fit Variables
double	a[nParam], siga[nParam], deltaa[nParam], da[nParam];				//parameters
double	bevbeta[nParam];	
double	alpha[nParam][nParam], dYda[nData][nParam];                         

 
char	title[128];

//-----------------------------Non-linear Fits ------------------------------------------------------------

double	chiSq1, chiSq2, chiSq3, chiOld, chiCut, stepSize;
//-------------------------- Fitting Function Definition --------------------------------

char	g_code1 = ' ';		//Set in ChooseFunction
int		g_m = 0;			//Must be set from MAIN
int		g_nPts = 0;			//Must be set from MAIN

double (*CalcFunct)(int, double);	

// -+------------------------------ Control Program Flow -------------------------
bool	g_linearFn = true;

void	SetLinear(bool trueOrfalse)   //Select routines and instructions for Linear or Non-linear Fittin
{
	g_linearFn = trueOrfalse;
}

//----------------------------------------------------------------------

// Run this routine once to set function and its constants.
// Add other functions and constants as needed.

void ChooseFunction(char funcCode, char code1Set)
{
	switch (funcCode)
	{
		case 'P':						//Power series polynomial	- Chapt_7  (linear)
			CalcFunct = PowerSeriesFunc;
		break;
		case 'L':						//Legendre polynomial		- Chapt_7	(linear)
			CalcFunct = LegendreFunc;	//g_code selects: 'a' all terms; 'e' even only.
		break;							
		case 'R':				//Radioactive decay - 2 exp + background (m = 5)	(non-linear)
			CalcFunct = TwoExponentials;	
		break;
		case 'M':
			CalcFunct = OneLorentz;	
		break;
		case 'N':
			CalcFunct = TwoLorentz;	
		break;
	}
	g_code1 = code1Set;
}

//--------------------------- Getting and Putting variables ------------------------------
int Getm()
{
	return (g_m);
}

void Putm(int m)
{
	g_m = m;
	return;
}

double GetParam(int i)
{
	return(a[i]);
}

void PutParam(int i, double aNew)
{
	a[i] = aNew;
}

void PutParamIncr(int i, double deltaaNew)
{
	deltaa[i] = deltaaNew;
}

void PutXYdY(int i, double xin, double yin, double dxin)
{
	x[i]	= xin;
	y[i]	= yin;
	sigY[i]	= dxin;
}

void InvertX()
{
	int i;
	for (i = 1; i<= g_nPts;  
		i++)  x[i]  = 1/sqr(x[i]);		//fitting with  1/r^2 as independent variable
	return;
}

void PutTitle(char sBuffer[])
{
strcpy(title, sBuffer);
}
	
void PutnPts(int nData)
{
	g_nPts=nData;
}

//-----------------------  Analytic Fitting Functions --------------------------------------------
//		Functions must be of the form a[1]*f1(x) + a[2]*f2(x) + ...
//		The routines calculate individual; terms, f1, f2, ...
//		They are combined into a single expression in in the finction FillYarray.

double PowerSeriesFunc(int k, double x)		//
{
	double y;
	int i;
	y = 1;
	if (k > 1)
		for (i=2; i <= k; i++)  y = x * y;
	return(y);
}

double LegendreFunc(int k, double x)          
//	Define separate terms in a series, y =  a0*L0(x) + a1*L1(x) + ..
//	Note-  m = 1 corresponds to zeroth order.
{
	double	funcComp[12];		//i.e., 0th thru 10th order
    int		i, kk;
    double	cosTheta;

	switch (g_code1)						//all terms or even only
		{
			case  'a': kk = k;		  		//'a' -Calculate all terms, but only return the even ones
			break;	
			case  'e': kk = 2*k - 1;		//'e' -Calculate and return all terms
			break;
		}
	cosTheta = cos(PI*x/180.);
	funcComp[1] = 1;	
	if (kk > 1)
	{
		funcComp[2] = cosTheta;
		if (kk > 2)
		{
			for (i = 3; i <= kk; i++)
			{
				funcComp[i] = ((2*i-1)*cosTheta*funcComp[i-1]-(i-1)*funcComp[i-2])/i;
			}
		}
	}
		return(funcComp[kk]);
}

//------------  Non-Linear Fitting Functions must be completely defined here--------------------

double TwoExponentials(int idummy, double x)	// Chapter 8
{	double y;

	y = a[1] + a[2]*Expf(x, a[4]) + a[3]*Expf(x, a[5]);
	return(y);
}	

double Expf(double x, double a) 
{	double z, arg;
		arg = fabs(x/a);			//Note - does not allow positive exponentials
	if (arg > 60.) 
			z = 0.;
	else
			z = exp(-arg);
	return (z);
}

double OneLorentz(int idummy, double x)				//Calculate one peak on background
{
	double yy;
	yy = a[1] + a[2]*x + a[3]*sqr(x) + a[4]*Lorentz(x, a[5], a[6]);
	return(yy);
}

double TwoLorentz(int idummy, double x)				//Calculate two peaks on background
{
	double yy;
	yy = a[1] + a[2]*x + a[3]*sqr(x) + a[4]*Lorentz(x, a[5], a[6])	                               
		                              + a[7]*Lorentz(x, a[8], a[9]);
	return(yy);
}

double Lorentz(double x, double mean, double gamma)  // Calculate the peak
	{
		double Lor;
		Lor = gamma/(2/PI)/(sqr(x-mean) + sqr(gamma/2));
		return(Lor);
	}

//------------------------------- General Purpose Functions ----------------------------
double yFunction(double x)
{
	double y;
	int k;

	if (g_linearFn)
	{
		y = 0.0;
		for (k=1; k<= g_m; k++) 		//Linear fitting of a sum of a[k]*f().
		y = y + a[k] * CalcFunct(k, x); //Beta and alpha require the individual f(x)'s.
	}
	else
		y = CalcFunct(0,x);				//Non-linear fitting calculates beta and alphs from derivatives ofXi2 and of f(x)
	return(y);
}

void FillYarray()  // Fills yCalc array
{
	int i;
	for (i= 1; i <= g_nPts; i++) 
	{
		yCalc[i] =  yFunction(x[i]);
	}
}
	
void StatisticalErrs()
{
	int i;
	for (i=1; i <= g_nPts; i++)
	{
		sigY[i] = sqrt(y[i]); 
	}
}

double CalcChiSq()
{
	int  i;     
	double Xi2;
	FillYarray();
	Xi2=0.;
	for (i = 1; i <=  g_nPts; i++)
			Xi2 = Xi2 + sqr((y[i]-yCalc[i]) / sigY[i]);
	return(Xi2);
}

//-----------Self-contained routine to  fit a Straight Line by Determinants  (Chapter 6) -----------
double LineFit()
{
	int i;        //  NOTE    dev    : text;
	double    det, sumWt, sumX, sumY, sumX2, sumY2, sumXY, weight;
	sumWt = 0;  sumX  = 0;  sumY  = 0;
	sumX2 = 0;  sumY2 = 0;  sumXY = 0;
//------- accumulate weighted sums -----------
	for (i= 1; i <= g_nPts; i++)
	{
		weight  = 1/sqr(sigY[i]);
		sumWt   = sumWt  + weight;
		sumX    = sumX   + weight * x[i];
		sumY    = sumY   + weight * y[i];
		sumX2   = sumX2  + weight * sqr(x[i]);
		sumY2   = sumY2  + weight * sqr(y[i]);
		sumXY   = sumXY  + weight * x[i]*y[i];
	}
 
// calculate the parameters - cut out if determinant is not > 0 }
	det =  sumWt * sumX2 - sumX * sumX;
	if (det > 0)
	{
		a[1]     = (sumX2*sumY  - sumX*sumXY)/det;
		a[2]     = (sumXY*sumWt - sumX*sumY) /det;
		siga[1]  = sqrt(sumX2/det);
		siga[2]  = sqrt(sumWt/det);
		return(det);
	}
	else
	return(-1);

}

// ----------------------- Matrix Fitting Routines added for CHAPTER 7 --------------------

// Set up the alpha and beta matrices for linear least-squares fitting.

void MakeBeta()  
{	
	int		i, j, k;   
	
	if (g_linearFn)
//--------------------- Set up the beta matrices for linear fitting 
	{	for (k=1; k <= g_m; k++)
			{
				bevbeta[k] = 0.;
				for (i = 1; i <= g_nPts; i++) 
					bevbeta[k] = bevbeta[k] + y[i]*CalcFunct(k, x[i])/sqr(sigY[i]);
			}
	}
	else

//-------------------- Set up the beta matrices for non-linear fitting - see Eq. 8.26	
	{
		for (j = 1; j <= g_m;j++)
		{
			bevbeta[j] = -dXiSq_da(j)/2.;
		}
	}
}

void MakeAlpha()               //Make the alpha matrices 
{
	int i,j,k;
    
	if (g_linearFn)
//--------------------- Set up the alpha matrices for linear fitting 
	{
		// cout << "Alpha Matrix" << endl;
		for (j = 1; j <= g_m;j++)
		{
			// cout << " j" << j << endl;
			for (k = 1; k <= g_m; k++)
			{
				alpha[j][k] = 0.;
				for (i = 1; i<=g_nPts; i++)
				{	
					alpha[j][k] = alpha[j][k]+CalcFunct(j, x[i])*CalcFunct(k, x[i])/sqr(sigY[i]);
				}	 // x[i]
			}   //k
		} //j
	
	} //if (g_linearFn)
	else
//--------------------- Set up the alpha matrices for non-linear fitting 
	{
		for (j = 1; j <= g_m; j++)
		{
			alpha[j][j] =  0.5 * d2XiSq_da2(j);
			if (alpha[j][j] == 0) 
			{
				ErrorMessage("Diagonal element is zero,");
				return;
			}
			if (j > 1) 
			{
				for (k = 1; k < j; k++)
				{
					alpha[j][k] =  0.5*d2XiSq_dajk(j,k);
					alpha[k][j] =  alpha[j][k];
				}  //for k
			}    //if j
		}      //for j
		for (j = 1; j <= g_m; j++)
		{
			if (alpha[j][j] < 0.0) 
			{
				alpha[j][j] = -alpha[j][j];
				if (j > 1) 
				{
					for (k = 1; k < j; k++)
					{
						alpha[j][k] = 0;
						alpha[k][j] = 0;
					} //for k
				}   //if j
			}     //if alpha
		}       //for j
	}// if if (g_linearFn)
}

// ----------------------- invert a matrix
double MatInv()	
{
	int i, j, k, L, ik[10], jk[10];
	double aMax, save, det;
	
	det = 0;
//------------------------------------  find largest element 
	for (k = 1; k <= g_m; k++)
	{
		aMax = 0;
FIND_AMAX:	
		for (i = k; i <= g_m; i++)
		{
			for (j = k; j <= g_m;j++)
			{
				if  (fabs(alpha[i][j]) > fabs(aMax)) 
				{
					aMax = alpha[i][j];
					ik[k] = i;
					jk[k] = j;
				}   //if
			}  //for j
		}  //for i
		if (aMax == 0)  return(det);  //with 0 determinant as signal
		det = 1;
// -------------------------------------- interchange rows and columns to put aMax in alpha[k,k]---
		i = ik[k];
		if (i < k) goto FIND_AMAX;
		else if (i > k) 
		{
			for (j = 1;j <= g_m;j++)
			{
				save = alpha[k][j];
				alpha[k][j] = alpha[i][j];
				alpha[i][j] = -save;
			}	  //for j
		}  //else if 
		j = jk[k];
		if	(j < k) goto FIND_AMAX;
		else if (j > k)
		{
			for (i = 1; i <= g_m; i++)
			{
				save = alpha[i][k];
				alpha[i][k] = alpha[i][j];
				alpha[i][j] = -save;
			}	 //for i
		}//else if j
// ---------------------------------------- accumulate elements of inverse matrix 
		for (i = 1; i <= g_m; i++)
		{
			if (i != k) alpha[i][k] = -alpha[i][k]/aMax;
		}	//for i
		for (i = 1; i <= g_m; i++)
		{
			for (j = 1; j <= g_m;j++) 
			{
				if ((i != k) && (j != k)) alpha[i][j] = alpha[i][j] + alpha[i][k]*alpha[k][j];
			}	//for j
		}   //for i
		for (j = 1; j <= g_m;j++)
		{
			if (j != k)  alpha[k][j] = alpha[k][j]/aMax;
		}           //for j
		alpha[k][k] = 1/aMax;
		det = det * aMax;
	} //for k
// ------------------------------------------ restore ordering of matrix 
	for (L = 1; L <= g_m; L++)  
	{
		k = g_m + 1 - L;
		j = ik[k];
		if (j > k) 
		{
			for (i = 1; i <= g_m; i++) 
			{
				save        = alpha[i][k];
				alpha[i][k] = -alpha[i][j];
				alpha[i][j] = save;
			}  //for i
		}  //if j
		i = jk[k];
		if (i > k)
		{
			for (j = 1; j <= g_m;j++)
			{
				save        =  alpha[k][j];
				alpha[k][j] = -alpha[i][j];
				alpha[i][j] =  save;
			} //for j
		} //if i
	}  //for L
	return(det);
}

void SquareByRow()  // multiply square matrix by row matrix
{
	int		i,j;
								
	for (i = 1; i <= g_m; i++) 
	{
		if(g_linearFn)
		{
			a[i]  = 0.0;
			for (j= 1; j<= g_m;j++)
				a[i] = a[i] + bevbeta[j]*alpha[i][j];
		}
		else
		{
			da[i] = 0.0;
			for (j= 1; j<= g_m;j++)
				da[i] = da[i] + bevbeta[j]*alpha[i][j];
		}
	}
		
}

// ----Standard deviations calc'd from chiSq change of 1 (parabola fit at Xi2 minimum)
void  SigParab()
{
	int j;

	for(j = 1; j<= g_m; j++)
	{
		chiSq2 = CalcChiSq();
		a[j]   = a[j] + deltaa[j];
		chiSq3 = CalcChiSq();
		a[j]   = a[j] - 2*deltaa[j];
		chiSq1 = CalcChiSq();
		a[j]   = a[j] + deltaa[j];
		siga[j] = deltaa[j]*sqrt(2/(chiSq1-2*chiSq2+chiSq3));
	}
}
// ---standard deviations as sqrt of diagonal elements of eror matrix.
void SigMatrix()
{
	int j;
	for (j = 1; j <= g_m;j++)		

		siga[j] = sqrt(alpha[j][j]);
}

//-------------------------  Non-linear Fits ------------------------------------

//Numerical Derivatives------------------------------
// Can be replaced by analytic derivatives, if they can be calculated. 
// However, numerical calculation is general, and convenient.
double dXiSq_da(int j)		//See Eq. 8.26 - this sums over nPts
{
	double  static XiSq0;
	double   XiSqPlus, dXiSqDa;

	if (j == 1)   
		XiSq0 = CalcChiSq();				//starting point-calculate it once
	a[j] = a[j] + deltaa[j];
	XiSqPlus  = CalcChiSq();
	a[j] = a[j] - deltaa[j];		//restore
	dXiSqDa   =  (XiSqPlus - XiSq0)/(deltaa[j]);
	return (dXiSqDa);
}

double d2XiSq_da2(int j) // See Eq. 8.35  - this sums over nPts
{
	double tem; 
	int i;

	if (j == 1) 
		for (i = 1; i<= g_nPts; i++) 
			y_0[i] = yFunction(x[i]);		//Starting point-calculate it once
	a[j] = a[j] + deltaa[j];
	tem = 0.0;
	for (i = 1; i <= g_nPts; i++)
		{
			dYda[i][j] = (yFunction(x[i]) - y_0[i])/deltaa[j]/sigY[i];
			tem  = tem + sqr(dYda[i][j]);
		}
		a[j] = a[j] - deltaa[j];
		return(2*tem);
}

double d2XiSq_dajk(int j, int k)	//See Eq. 8.35
{
	double tem = 0.0;
	int i;
	for (i = 1; i <= g_nPts; i++)
	{
 		tem = tem + dYda[i][j]*dYda[i][k];
	}
	return(2*tem);
}

// ====================  Non-linear Fitting Routines ==================================
//---------------------------- GridSearch ---------------------------------------------

// Program 8.1:Non-linear least-squares fit by the grid-search method

void Gridls(double &chiSqr)
{
//
	double delta;
	double save, delta1, del1, del2, aa, bb, cc, disc, alpha, x1, x2;
	int j;

	//cout << "enter Grids, x[1], y[1] " <<x[1] <<"  "<<y[1]<<"  ";  cin >> j;

	chiSq2 = CalcChiSq();
// -find local minimum for each parameter- 
	for (j = 1; j <= g_m;j++)
    {
		delta    = deltaa[j];
		a[j]     = a[j] + delta;
		chiSq3   = CalcChiSq();
		if (chiSq3 > chiSq2) 
		{                  //started in wrong direction
			delta  = -delta;
			a[j]   =  a[j] + delta;
			save   =  chiSq2;    //interchange 2 and 3 so 3 is lower
			chiSq2 =  chiSq3;
			chiSq3 =  save;
		}
// -Increment or decrement a[j] until chi squared increases- 
		do
		{
			chiSq1 = chiSq2; //move back to prepare for quad fit
			chiSq2 = chiSq3;
			a[j]   = a[j] + delta;
			chiSq3 = CalcChiSq();
		}  while (chiSq3 < chiSq2);
   
// -Find minimum of parabola defined by last three points  -
		del1 = chiSq2 - chiSq1;
		del2 = chiSq3 - 2*chiSq2 + chiSq1;
		delta1 = delta * (del1/del2 + 1.5);
		a[j] = a[j]  - delta1;
		chiSq2 = CalcChiSq();    //	at new local minimum
// -Adjust delta for change of 2 from chiSq at minimum  -
		aa = del2/2;								//chiSq = aa*sqr(a[j] + bb*a[j] + cc
		bb = del1 - del2/2;
		cc = chiSq1-chiSq2;
		disc = sqr(bb) -4*aa*(cc-2);				//chiSqr difference=2
		if (disc > 0) 								//if not true, then probably not parabolic yet
		{
			disc = sqrt(disc) ;
			alpha = (-bb - disc)/(2*aa);
			x1 = alpha*delta +  a[1] - 2*delta;		//	a[j] at chiSq minimum+2
			disc = sqr(bb) - 4*aa*cc;
			if (disc > 0) 
				disc=sqrt(disc); 
			else 
				disc=0;		// elim round err
			alpha = (-bb - disc)/(2*aa);
			x2 = alpha*delta + a[1] - 2*delta;		// at chiSq minimum
			delta = x1 - x2;
			deltaa[j] = delta;
		}
	}    // for j = 1 to m}
  chiSqr = chiSq2;
}

//---------------------------- GradSearch ---------------------------------------------
//Program 8.2 Non-linear least-squares fit by gradient-search method}

	double	grad[10];
void Gradls(double &chiSqr, double stepDown)
// label  5;
{
	double stepSum,step1;
	int   j;

	CalcGrad();   //calculate the gradient
//-Evaluate chiSqr at new point and make sure chiSqr decreases-
	do
	{
		for (j = 1; j <= g_m;j++)
			a[j] = a[j] + stepDown * grad[j]; //slide down
		chiSq3 = CalcChiSq();
		if (chiSq3 >= chiSq2)
		{                         //must have overshot minimum
			for (j = 1; j <= g_m;j++)
				a[j] = a[j] - stepDown * grad[j]; //restore
			stepDown = stepDown/2;              //decrease stepSize
		}
	} while (chiSq3 > chiSq2);
	stepSum = 0;
// -- Increment parameters until chiSqr starts to increase -- 
	do
	{
		stepSum = stepSum + stepDown;   //counts total increment
		chiSq1 = chiSq2;
		chiSq2 = chiSq3;
		for (j = 1; j <= g_m;j++) 
			a[j] = a[j] + stepDown * grad[j];
		chiSq3 = CalcChiSq();
	} while (chiSq3 <= chiSq2);
// -- Find minimum of parabola defined by last three points -- 
	step1=stepDown*((chiSq3-chiSq2)/(chiSq1-2*chiSq2+chiSq3)+0.5);
	for (j = 1; j <= g_m;j++)
		a[j] = a[j] - step1 * grad[j];    //move to minimum
	chiSqr = CalcChiSq();
	stepDown = stepSum;                 //start with this next time
}

void CalcGrad()
{
	double fract = 0.001;
	int   j;
	double  dA, sum;

	sum = 0.0;
	chiSq2 = CalcChiSq();

	for (j = 1; j <=  g_m;j++)
  {
		dA  = fract * deltaa[j];     //differential element for gradent
		a[j]    = a[j] + dA;
		chiSq1  = CalcChiSq();
		a[j]    = a[j] - dA;
		grad[j] = chiSq2 - chiSq1;   //2*da*grad
		sum     = sum + sqr(grad[j]);
  }
	for (j = 1; j <= g_m;j++)
		grad[j] =  deltaa[j]*grad[j]/sqrt(sum); //step * grad
}

//----------------------------------- Expand Function -------------------------------------
//Program  8.3: Non-linear least-squares fit by expansion of the fitting function
 
void ChiFit(double &chiSqr)			// double  det, chiSq1;
{
	int  j;

	MakeBeta();
	MakeAlpha();
	(void)MatInv();					// Invert matrix
	SquareByRow();				// Evalulate parameter increments
	for (j = 1; j <= g_m;j++)
		a[j] = a[j] + da[j];	// Increment to next solution. 
	chiSqr= CalcChiSq();
	return;
}

// ------------------------------------- Marquard ---------------------------------------------
//Program 8.4: Non-linear least-squares fit by the gradient-expansion (Marquardt) method

void Marquardt(double &chiSqr, double chiCut, double lambda)
{
	int  j;
	double  chiSq1;

TOP:
	MakeBeta();
	MakeAlpha();
	for (j = 1; j <= g_m;j++) 
		alpha[j][j] = (1 + lambda) * alpha[j][j];
	(void)MatInv();					// Invert matrix
	if (lambda > 0)					//On final call, enter with lambda = 0 to get the error matrix
	{
		SquareByRow();//Evaluate parameter increments 
		chiSq1 = chiSqr;
		for (j = 1; j <= g_m; j++)
			a[j] = a[j] + da[j];	//Incr to next solution
		chiSqr = CalcChiSq();
		if (chiSqr > (chiSq1 + chiCut) )
		{
			for (j = 1; j <= g_m; j++)
				a[j]=a[j]-da[j];	//Return to prev solution
			chiSqr = CalcChiSq();
			lambda = 10*lambda;		//and repeat the calc, with larger lambda
			goto TOP;
		}
		lambda = 0.1 * lambda;
	}
}

// --------------------------------  Output to Disk File -------------------------------------------

void FitOut(char outFile[], double chi2, char aErrorsFrom)
{
	int i, k, j ,nFree;
	double X2Prob, chi2PerDof;

	ofstream fOut(outFile);			// open file for output 
	fOut << setiosflags(ios::fixed |ios::showpoint);
	
	nFree = g_nPts-g_m;
	chi2PerDof = chi2/nFree;
	X2Prob = 100*ChiProb(nFree, chi2);
	switch(aErrorsFrom)
	{
		case 'D':			// already calculated in LineFit
			;			
		break;
		case 'C':			// vary chi^2 by 1
			SigParab();		
		break;
		case 'M':			// error Matrix
			SigMatrix();
		break;
	}

	fOut << title <<endl; 
	fOut 
		<< " ChiSqr =" 
		<< setprecision(1)<<chi2 
		<< " for " 
		<< nFree 
		<<" deg of freedom," 
		<<" chiSqr/dof =" 
		<< setprecision(1)<<chi2PerDof 
		<< " Prob =" 
		<< setprecision(1)<< X2Prob
		<<"%"
		<< endl;
	fOut 
		<< " Fitted Parameters:  a[i] +- sig-a[i]"
		<< endl;
	for (i = 1; i <= g_m; i++)  	
		fOut 
			<<setprecision(4)<<setw(10) << a[i]
			<<"  +-  "
			<<setw(10)<< siga[i]
			<< "  " 
			<< endl;
	fOut << endl; 	

	if (!(aErrorsFrom == 'M'))
		cout <<  endl <<" Output written to disk file " << outFile << endl;
	else
	{
		fOut << " Error Matrix" << endl;
		for (k = 1; k <= g_m; k++)
		{
			for (j = 1; j <= g_m;j++)
			{ 
				fOut 
					<<"    "
					<< setprecision(8) <<setw(12) 
					<< alpha[k][j];
			}
			fOut << endl;
		}
		fOut << endl;
		cout << endl <<" Output written to disk file " << outFile << endl;
	}

// Tabulate data with fitted Y
	fOut << "      pt #             X(cm)      Y            dY         yCalc " << endl;
	for (i = 1; i <= g_nPts; i++)  
	{
			fOut 
			<< setiosflags( ios::right )
			<< setprecision(1)<<setw(10)<< i 
			<< setprecision(4)
			<< setw(16)<< x[i] 
			<< setw(12)<< y[i]
		    << setw(12)<< sigY[i]
			<< setw(12)<< yCalc[i] 
			<< endl;
	}
	fOut.close();
}

