//  c:\cpp\Cpp\Library\FitUtil  -  Utilities for Least Squares Fitting


#include <fstream.h>
#include <iomanip.h>
#include <math.h>
#include <string.h>

#include "c:\cpp\CppLibrary\GenUtil.h"
#include "FitUtil-ch9.h"
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
{
	switch (funcCode)
	{
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


//------------  Non-Linear Fitting Functions must be completely defined here--------------------


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


// ----------------------- Matrix Fitting Routines added for CHAPTER 7 --------------------


// Set up the alpha and beta matrices for non-linear least-squares fitting.

void MakeBeta()  
{	
	int		i, j;   	
	for (j = 1; j <= g_m;j++)
	{
		beta[j] = -dXiSq_da(j)/2.;
	}
	
}

void MakeAlpha()               //Make the alpha matrices 
{
	int i,j,k;
	
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


void SquareByRow()  // multiply square matrix by row matrix- (non-linear uses increments)
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

// ------------------------- Non-linear Fitting Routine  =-Marquard method---------------------------------------
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

