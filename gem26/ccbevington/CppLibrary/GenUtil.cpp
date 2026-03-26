// General Utilities - mainly I/O
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "GenUtil.h"  
#include "FitUtil.h"


//-------------------------------Read Disk Files ----------------------------------
int    iBuffer[40];
double dBuffer[20];

char	*fileName;
const int cLength = 127;
char   cBuffer[cLength];

static streampos iFpoint;

void OpenInFile(char *Fname, int length)
{	
	iFpoint = 0;

	fileName = new char[length];
	if (fileName == NULL)
	{
		cout << "cannot allot fileName" << endl;
		exit(EXIT_FAILURE);
	}
	strcpy(fileName, Fname);
}
// ----------------------------STRINGS--------------------
void ReadString(char *sBuffer, int sLength)
{
//----------------------------------
	int nChars, i;		

	ifstream inFile(fileName);		// open file for STRING
	if (!inFile)
	{
		cout << "Failed to open file for STRING" << endl;
		exit(EXIT_FAILURE);
	};
	inFile.seekg(iFpoint);
//----------------------------------
	
	inFile.getline(cBuffer,cLength, '\n');
	nChars = inFile.gcount();
	iFpoint = inFile.tellg();
	for (i = 0; i <= nChars; i++) *(sBuffer+i) = cBuffer[i];
 	        *(sBuffer+ nChars+1) = '\0';
	inFile.close();
	return;

}
	
// ----------------------------INTEGERS--------------------

void ReadIntegers(int numNumbers,int *Buffer,int iLength) 		
{
	int i;
//----------------------------------
	ifstream inFile(fileName);		// open file for INTEGERS	
	if (!inFile)
	{
		cout << "Failed to open file for INTEGERS" << endl;
		exit(EXIT_FAILURE);
	};
	inFile.seekg(iFpoint);  
//----------------------------------
	for (i = 1; i<= numNumbers; i++)
	{
		inFile >> iBuffer[i];
	}
	iFpoint = inFile.tellg();
	for (i = 1; i <= numNumbers; i++)   *(Buffer+i) = iBuffer[i];
	inFile.close();
	return;
}

// ----------------------------DOUBLES--------------------

void ReadDoubles(int numNumbers,double *Buffer,double dLength)
{
	int i;
//----------------------------------
	ifstream inFile(fileName);		// open file for DOUBLRS
	if (!inFile)
	{
		cout << "Failed to open file fo DOUBLES" << endl;
		exit(EXIT_FAILURE);
	};
	inFile.seekg(iFpoint);
//----------------------------------
	for (i=1; i<= numNumbers;i++) 
	{	
		inFile >> dBuffer[i];
	}
	iFpoint = inFile.tellg();
	for (i = 1; i <= numNumbers; i++)   *(Buffer+i) = dBuffer[i];
	inFile.close();
	return;
}
	
//-------------------------------------------------------------

void ErrorMessage(const char message[])
{
	cout << endl;
	cout << message << endl;
}


char pause()		//--- pause routine for ch 6	
{
	char test;
	cout << "Press Enter to continue " << flush;
	test = getchar();
	//cin >> test;
//	if (test== 'Q' || test== 'q' )
	return('K');

}
double sqr(double x)	
{
	x = x * x;
	return (x);
}


//-------------------------------- Useful Functions ---------------------------- 

//----Simpson's rule  args: funct selects the function, ... p1, and p2 are parameters of the function.
double Simpson(char functCode,int nIntervals, double loLim, double hiLim, double p1, double p2, double p3)   //2 calcs/interval
{
	double (*SimpsonFunct)(double, double, double);
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
//		case 'G':
//		SimpsonFunct = Gauss_Dens;
//		break;
	}
	sum = SimpsonFunct(loLim, p1, p2) - SimpsonFunct(hiLim, p1, p2);
	for (i=1; i <= nIntervals; i++) 
	{
		x = x + 2*dx;

		t1 = SimpsonFunct(x-dx, p1, p2);
		t2 = SimpsonFunct(x,    p1 ,p2);
		sum = sum + 4*t1 + 2*t2;
//		cout << x << "   " << t2 << "  " << sum <<  endl;
		x = x;
	}
	sum = sum*dx/3;
	return(sum);
}



double Power(double x, double a)	//x^a
{
	double tem;

	tem = exp(a*log(x));
	return(tem);
}


//-------------------------------- Chi^2 Density and Probability ---------------------------- 
double Chi2_Dens(double x, double h, double g) //See ChiProb for g.
{
	double chiDens;
	
	if (x == 0.) 
		chiDens = 0.; 
	else
		chiDens = Power(x, h-1)*exp(-x/2.)/g;		//  x^(h-1) e^(-x/2);
	return(chiDens);
}

double	ChiProb(double nFree, double chi2)
{
	double	probTem,  
			cLim = 2,								// expansion limit for nFree = 1
			intFromLim = 0.157,						// integral from cLim to infInity for nFree = 1
			dx = 0.2,
			g; 				// determines accuracy of integration}
	int nIntervals = chi2/dx;		// number  of intervals for Simpson integral.  2 calculations per interval.
	double h =  nFree/2.;

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


double Gamma(double h)  //Approximate Gamma function for integers and half-integers     
{ 

	double gamma;
//gamma  = sqrt(2*PI)*exp(-h)*[h^(h-1/2)]*[1 + 0.0833/h]  with h = nFree/2 }

	gamma = sqrt(2*PI)*exp(-h+(h-0.5)*log(h))*(1 + 0.0833/h);

//	gamma = sqrt(2*PI)*exp(-h)*Power(h, h-0.5) * (1 + 0.0833/h);

	return(gamma);
}






