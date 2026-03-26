// FitUtil-ch7  -  Utilities for Least Squares Fitting

#include <fstream.h>
#include <iomanip.h>
#include <math.h>
#include <string.h>

#include "c:\cpp\CppLibrary\GenUtil.h"
#include "FitUtil-ch7.h"

//---------------------------- Data and Parameter Arrays ----------------------------------
const	int nParam = 10;		// max number of parameters
const	int nData = 100;		// max number of data coordinates
double	x[nData], y[nData], sigY[nData], yCalc[nData], y_0[nData];	// fit Variables
double	a[nParam], siga[nParam], deltaa[nParam], da[nParam];				//parameters
double	beta[nParam];	
double	alpha[nParam][nParam], dYda[nData][nParam];                         

 
char	title[128];

//-----------------------------Non-linear Fits ------------------------------------------------------------

//z-------------------------- Fitting Function Definition --------------------------------

char	g_code1 = ' ';		//Set in ChooseFunction
int		g_m = 0;			//Must be set from MAIN
int		g_nPts = 0;			//Must be set from MAIN

double (*CalcFunct)(int, double);	

// Run this routine once to set function and its constants.  
// Add other functions and constants as needed.

void ChooseFunction(char funcCode, char code1Set)
{
	switch (funcCode)
	{
		case 'P':						// Power series polynomial	- Chapt_7  (linear)
			CalcFunct = PowerSeriesFunc;
		break;
		case 'L':						// Legendre polynomial		- Chapt_7	(linear)
			CalcFunct = LegendreFunc;	// g_code1 selects ='a' for all terms; 'e' for even only
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

  //------------------------------- General Purpose Functions ----------------------------
double yFunction(double x)
{
	double y;
	int k;

	y = 0.0;
	for (k=1; k<= g_m; k++) 		//Linear fitting of a sum of a[k]*f().
	y = y + a[k] * CalcFunct(k, x); //Beta and alpha require the individual f(x)'s.
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

void MakeBeta()			// Make beta matrices
{	
	int		i,  k;   
	
///--------------------- Set up the beta matrices for linear fitting 
	for (k=1; k <= g_m; k++)
	{
		beta[k] = 0.;
		for (i = 1; i <= g_nPts; i++) 
			beta[k] = beta[k] + y[i]*CalcFunct(k, x[i])/sqr(sigY[i]);
	}
	
}

void MakeAlpha()               //Make the alpha matrices 
{
	int i,j,k;
    
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


void SquareByRow()  // multiply square matrix by linear matrix
{
	int		i,j;
								
	for (i = 1; i <= g_m; i++) 
	{
		a[i]  = 0.0;
		for (j= 1; j<= g_m;j++)
			a[i] = a[i] + beta[j]*alpha[i][j];
		
	}
							
}

// ---standard deviations as sqrt of diagonal elements of eror matrix.
void SigMatrix()
{
	int j;
	for (j = 1; j <= g_m;j++)		

		siga[j] = sqrt(alpha[j][j]);
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
	X2Prob = 100*ChiProb(nFree,chi2);
			
			SigMatrix();	//Calc errors in parameters from the error matrix.

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

