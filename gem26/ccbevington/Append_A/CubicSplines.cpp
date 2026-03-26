// Sample Spline Interpolation

#include <fstream.h>
#include <iomanip.h>
#include <math.h>
#include <stdlib.h>
#include "c:\cpp\Chapt_6\GenUtil.h"


	// Input Data
	const int dataArray = 100;	
	double xx[dataArray],yy[dataArray], d2ydx2[dataArray];  
	int		nPoints;		
	double	h;				
						
void SplineMake(int nn, double d2ydx2A, double d2ydx2B, double xIn[], double yIn[]) // Makes a table of 2nd derivatives
{  
	int i;
	double a[dataArray], delt1[dataArray], delt2[dataArray], b[dataArray];

	nPoints = nn;									// This makes nPoints available to function SplineInt
	h = (xIn[nPoints] - xIn[1])/(nPoints-1);
	for	(i = 1; i <= nPoints; i++)
	{
		xx[i] = xIn[i];
		yy[i] = yIn[i];
	}
	d2ydx2[1] = d2ydx2A;										// values of 2nd derivatives were provided on input
	d2ydx2[nPoints] = d2ydx2B;
	a[2] = 4;
	for (i = 3; i <= nPoints-1; i++) 
		a[i] = 4-1/a[i-1];										// coefficients     
	for (i = 2; i <= nPoints; i++)								// 1st differences  
		delt1[i] = yIn[i] - yIn[i-1];
	for (i =  2; i <= nPoints-1; i++)							// 2nd differences x 6 
		delt2[i] = 6*(delt1[i+1] - delt1[i])/(h*h);
	b[2] = delt2[2] - d2ydx2[1];;           // b coefficients   
	for (i= 3; i <= nPoints-1; i++)
		b[i] = delt2[i] - b[i-1]/a[i-1];
	b[nPoints-1] = b[nPoints-1] - d2ydx2[nPoints];
	d2ydx2[nPoints-1] = b[nPoints-1]/a[nPoints-1];
	for (i = nPoints-2; i >= 2; i--)
		d2ydx2[i] = (b[i] - d2ydx2[i+1])/a[i];					// 2nd derivatives  
}
double DyDx(int i)												// first derivative  
{
	double dydx;
    dydx = (yy[i+1]-yy[i])/h - h*(d2ydx2[i]/3 + d2ydx2[i+1]/6);
	return(dydx);
 }

 double D3yDx3(int i)											//third derivative
 {
	double	d3ydx3;
	d3ydx3 = (d2ydx2[i+1] - d2ydx2[i])/h;
	return(d3ydx3);
  }
	
double SplineInt(double x)
{
	double	dx, splineInt;
	int		i;

	i = ((x-xx[1])/h)+1;
	if (i < 1)		
		i = 1;
	if (i > nPoints-1)
		i = nPoints-1; 
	dx = x -xx[i] ;
    // interpolate x 
	if (i == nPoints)		
		splineInt = yy[i];
	else 
		splineInt = yy[i] + (DyDx(i) + (d2ydx2[i]/2 + D3yDx3(i)/6*dx)*dx)*dx;
	return(splineInt);
}
double xpl[20], ypl[20];

//----------------------------------------------------------------------------------------------------
	char FileName[] = "spline.dat";
	double				d2a, d2b, xs;
	char		Title[] = "                                                                    ";
	int			iiBuffer[30];
	double		ddBuffer[30];
	double		interpolated_y;
	int j, nInts, nData, nDbls;

// --------------------------------------Fetch Data ---------------------------------------------
	void FetchData()
	{
		OpenInFile(FileName, sizeof(FileName)/sizeof(char));			
	ReadString(Title, sizeof(Title)/sizeof(char));
	cout << Title << endl;
	nInts = 1;
	ReadIntegers(nInts, iiBuffer, sizeof(iiBuffer)/sizeof(int));
	nData = iiBuffer[1];											// Get number of data points
	nDbls = 2;
	ReadDoubles(nDbls, ddBuffer, sizeof(ddBuffer)/sizeof(double));	// The test file has two 2nd derivatives as the first line.
	d2a = ddBuffer[1]	;		// 2nd deriv at lower boundary 
	d2b = ddBuffer[2];			// 2nd deriv at upper boundary
	cout << "Input Data(X, Y)   [2nd derivs at lower and upper bounds = " << d2a << "   " <<d2b << ']' << endl;  
	for (j = 1;j<= nData; j++)
	{
		ReadDoubles(nDbls, ddBuffer, sizeof(ddBuffer)/sizeof(double));	// Now, read the data
		xpl[j] = ddBuffer[1];
		ypl[j] = ddBuffer[2];
		cout << xpl[j] << "    " << ypl[j]  << endl;
	}
	}

// Test Routine for Cubic Spline Interpolation ---------------------------------------

void main()
{
	FetchData();
	xs = 1.;
	while (xs > 0)
	{
	SplineMake(nData, d2a, d2b, xpl, ypl);								// Make the 2nd derivatives
	cout << "Type a value of X  ";
	cin >> xs;
	interpolated_y = SplineInt(xs);										// Interpolate
	cout <<  "Interpolated y = " << interpolated_y  << endl;
	}
	return;
}
