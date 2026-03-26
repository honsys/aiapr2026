//Program 8.0: Main calling routine for testing non-linear fitting methods
//Requires Matrix from Chapt-7
//program NonLinFt

#include <fstream.h>
#include <math.h>
#include <iomanip.h>
#include <stdlib.h>
#include "FitUtil-ch8.h"
#include "c:\cpp\CppLibrary\GenUtil.h"

char sBuffer[] = "  ";
int iiBuffer[30];
double ddBuffer[30];

char DataFile1[]  = "RadioDk.dat";
char ParamFile1[] = "RadioDk.par";


void ReadFitData(int nDbls)
{		
	int j, nInts, nData;

	ReadString(sBuffer, sizeof(sBuffer)/sizeof(char));
	PutTitle(sBuffer);
//	cout << "sBuffer  " << sBuffer << endl; 
	nInts = 1;
	ReadIntegers(nInts, iiBuffer, sizeof(iiBuffer)/sizeof(int));
	nData = iiBuffer[1];
//	cout << "nData = " << nData << endl;
	PutTitle(sBuffer);
	PutnPts(nData);
	for (j = 1;j<= nData; j++)
	{
		ReadDoubles(nDbls, ddBuffer, sizeof(ddBuffer)/sizeof(double));
		PutXYdY(j, ddBuffer[1], ddBuffer[2], ddBuffer[3]);
//		cout << ddBuffer[1] << "  " << ddBuffer[2] << "  " << ddBuffer[3]  << endl;
	}		
}

void ReadFitParams(double stepsize)
{
		int j, nParams;
		double aParam;

	ReadIntegers(1, iiBuffer, sizeof(iiBuffer)/sizeof(int));
	nParams = iiBuffer[1];
	Putm(nParams);
	cout << "nParams = " << nParams << endl;
	for (j = 1;j<= nParams; j++)
		{
			ReadDoubles(1, ddBuffer, sizeof(ddBuffer)/sizeof(double)); 
			aParam = ddBuffer[1];
			PutParam(j, aParam);						 // 1 parameter per line
//			cout  <<  ddBuffer[1] << "  ";
			PutParamIncr(j, aParam*stepsize);
		}
//		cout << endl;
}
	
void main()
{
	double stepScale[5] = {0, 0.49999, 0.99999, 0.001, 0.001};//stepScale[0] not used
	double chiCut   = 0.01;
	double stepDown = 0.1;						//step down the gradient in Gradls
	double lambda   = 0.001;					// for Marquardt fitMethod only	
	double	chiSqr, chiOld;
	int		trial, j, m, fitMethod;
	
//   Note that new functions must be included in the 'Functions' section of \Chapt_6\FitUtil,cpp,
//      and in the prototype header file, \Chapt_6\FitUtil.h.

	SetLinear(false);		//Non-linear fitting

	cout << "(1)Grid Search,(2)Gradient Search,(3) ChiSq Expansion,(4)Function Expansion" << endl;
	cout << "Type 1, 2, 3, or 4  to select fitting method  ";
	cin >> fitMethod;  
	OpenInFile(ParamFile1, sizeof(ParamFile1)/sizeof(char));		//"RadioDk.par"	
	ReadFitParams(stepScale[fitMethod]);												
	OpenInFile(DataFile1, sizeof(DataFile1)/sizeof(char));		//"RadioDk.dat"		
	ReadFitData(3);												// Statistical errors are in the file
	ChooseFunction('R',' ');									//'R' is the function code					
	m = Getm();
	trial	= 0;
	chiSqr	= CalcChiSq();	
	do
	{
		chiOld = chiSqr;
		cout <<"Trial #  "<< setw(4) << trial <<" chiSq= " <<setw (10 ) 
			<< setprecision(4) << setiosflags( ios::right )<< chiSqr ;
		for (j = 1; j <= m; j++)
			cout << setprecision(4) << setw(8) << GetParam(j) ;
			cout << endl;
		switch (fitMethod)
		{
			case 1: 
				Gridls(chiSqr);
			break;
			case 2: 
				Gradls(chiSqr, stepDown);
			break;
			case 3: 
				ChiFit(chiSqr);
			break;
			case 4: 
				Marquardt(chiSqr, chiCut, lambda);
			break;
		}			
		trial++;
	}   
	while (fabs(chiOld-chiSqr) >= chiCut); 

	FillYarray();
	chiSqr = CalcChiSq();

	switch (fitMethod)
	{
		case 1:
			FitOut("RDecay-Grid Search.txt", chiSqr, 'C');	//'C' - Example 8.1 - vary chi^2 for parameter errors
		break;
		case 2:
			FitOut("RDecay-Grad Search.txt", chiSqr, 'C');	//'C' - Example 8.2 - vary chi^2 for parameter errors
		case 3:
			FitOut("RDecay-Expnd Func.txt", chiSqr, 'M');	//Example 8.1 - parameter errors from Matrix Inversion
		break;
		case 4:
			FitOut("RDecay-Marqardt.txt", chiSqr, 'M');		//Example 8.1 - parameter errors from Matrix Inversion
		break;
	}
}