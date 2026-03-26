//  FitUtil  -  Utilities for Least Squares Fitting

//#include <stdarg.h
//#include <stdio.h>

#include <fstream.h>
#include <iomanip.h>
#include <math.h>
#include <string.h>

#include "C:\Cpp\CppLibrary\GenUtil.h"
#include "FitUtil-ch6.h"
//================================= Fitting Variables ==============================================

//---------------------------- Data and Parameter Arrays ----------------------------------
const	int nParam = 10;		// max number of parameters
const	int nData = 100;		// max number of data coordinates
double	x[nData], y[nData], sigY[nData], yCalc[nData], y_0[nData];	// fit Variables
double	a[nParam], siga[nParam];				//parameters

char	title[128];

//-------------------------- Fitting Function Definition --------------------------------

int		g_m = 0;			//Must be set from MAIN
int		g_nPts = 0;			//Must be set from MAIN


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

void PutXYdY(int i, double xin, double yin, double dxin)
{
	x[i]	= xin;
	y[i]	= yin;
	sigY[i]	= dxin;
}

void PutTitle(char sBuffer[])
{
	strcpy(title, sBuffer);
}
	
void PutnPts(int nData)
{
	g_nPts=nData;
}
	
void InvertX()
{
	int i;
	for (i = 1; i<= g_nPts;  
		i++)  x[i]  = 1/sqr(x[i]);		//fitting with  1/r^2 as independent variable
	return;
}

//-----------------------  Analytic Fitting Function --------------------------------------------
//		Functions must be of the form a[1]*f1(x) + a[2]*f2(x) + ...

double PowerSeriesFunc(double x)		//f[k] = a[k]*x^(k-1)
{
	double y = 0;
	double xp =1;
	int k;

	y = 0;
	for (k=1; k <= g_m; k++)  
	{
		y = y + a[k]*xp;
		xp = x*xp;
	}
	return(y);
}

void FillYarray()  // Fills yCalc array
{
	int i;
	for (i= 1; i <= g_nPts; i++) 
	{
		yCalc[i] =  PowerSeriesFunc(x[i]);
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

//-----------Self-contained routine to  fit a Straight Line by Determints  (Chapter 6) -----------
double LineFit()
{
	int i;
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


// ----------------------------------------------  Output to Disk File -------------------------------------------

void FitOut(char outFile[], double chi2, char aErrorsFrom)
{
	int i, nFree;
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

		fOut << endl;
		cout << endl <<" Output written to disk file " << outFile << endl;

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

