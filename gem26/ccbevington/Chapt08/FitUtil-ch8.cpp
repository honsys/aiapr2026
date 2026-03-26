//  c:\cpp\Cpp\Library\FitUtil  -  Utilities for Least Squares Fitting


#include <fstream.h>
#include <iomanip.h>
#include <math.h>
#include <string.h>

#include "c:\cpp\CppLibrary\GenUtil.h"
#include "FitUtil-ch8.h"
//================================= Fitting Variables ==============================================

//---------------------------- Data and Parameter Arrays ----------------------------------
const	int nParam = 10;		// max number of parameters
const	int nData = 100;		// max number of data coordinates
double	x[nData], y[nData], sigY[nData], yCalc[nData], y_0[nData];	// fit Variables
double	a[nParam], siga[nParam], deltaa[nParam], da[nParam];				//parameters
double	beta[nParam];	
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
{			CalcFunct = TwoExponentials; //Radioactive decay - 2 exp+bknd (m=5, non-linear)
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

//------- Non-Linear Fitting Functions -Must be completely defined here----------

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

//------------------------------- General Purpose Functions ----------------------------
double yFunction(double x)
{
	double y;
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

// ----------------------- Matrix Fitting Routines added for CHAPTER 7 --------------------

// Set up the alpha and beta matrices for non-linear least-squares fitting.

void MakeBeta()  
{		
	int j;
	for (j = 1; j <= g_m;j++)
	{
		beta[j] = -dXiSq_da(j)/2.;
	}

}

void MakeAlpha()               //Make the alpha matrices 
{
	int j,k;
    
//--------------------- Set up the alpha matrices for linear fitting 

	
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
		da[i] = 0.0;
		for (j= 1; j<= g_m;j++)
			da[i] = da[i] + beta[j]*alpha[i][j];
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
	double fract = 0.001;
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
	double det;

	MakeBeta();
	MakeAlpha();
	det = MatInv();					// Invert matrix
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
	double  det, chiSq1;

TOP:
	MakeBeta();
	MakeAlpha();
	for (j = 1; j <= g_m;j++) 
		alpha[j][j] = (1 + lambda) * alpha[j][j];
	det = MatInv();					// Invert matrix 
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


// ----------------------------------------------  Output to Disk File -------------------------------------------

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
		case 'C':			// vary chi^2 by 1, for Grid and Grad searches 
			SigParab();		
		break;
		case 'M':			// error Matrix
			SigMatrix();
		break;
	}

	fOut << title <<endl; 
	fOut ;
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

