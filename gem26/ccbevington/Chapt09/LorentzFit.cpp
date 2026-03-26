//Examples 9.1 (Prpgram 9.1) and 9.2: Uses the Marquardt fitting method
//Requires Matrix from Chapt-7
//program NonLinFt

#include <fstream.h>
#include <math.h>
#include <iomanip.h>
#include <stdlib.h>
#include "FitUtil-ch9.h"
#include "c:\cpp\CppLibrary\GenUtil.h"

char   sBuffer[] = "                                                                         ";
int    iiBuffer[30];
double ddBuffer[30];

//char DataFile1[]  = "c:\cpp\Chapt_9\Lorentz\radioDk.dat";
//char ParamFile1[] = "c:\cpp\Chapt_9\Lorentz\radioDk.par";
char DataFile1[]  = "radioDk.dat";
char ParamFile1[] = "radioDk.par";
char DataFile2[]  = "single.dat";
char ParamFile2[] = "single.par";
char DataFile3[]  = "double.dat";
char ParamFile3[] = "double.par";


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
			//cout  <<  ddBuffer[1] << "  ";
			PutParamIncr(j, aParam*stepsize);
		}
//	cout << endl;
}

		
void main()
{
	double stepScale =  0.001;  
	double chiCut   = 0.01;
	double stepDown = 0.1;						//step down the gradient in Gradls
	double lambda   = 0.001;					// for Marquardt fitMethod only	
	double	chiSqr, chiOld;
	int		trial, j, m;
	char fCode;
	
//   Note that new functions must be included in the 'Functions' section of \Chapt_6\FitUtil,cpp,
//    and in the prototype header file, \Chapt_6\FitUtil.h.

	cout <<"Type  '1' for 1 Lorentz peak, '2' for 2 peaks ";	// Which problem?
	cin >>  fCode;
	SetLinear(false);		//Non-linear fitting

	switch (fCode)
	{				

		case '1':	// A single Lorentzian on a parabolic background (6 parameters)
			OpenInFile(ParamFile2, sizeof(ParamFile2)/sizeof(char));		// "Single.par"	
			ReadFitParams(stepScale);												
			OpenInFile(DataFile2, sizeof(DataFile2)/sizeof(char));		// "Single.dat"		
			ReadFitData(2);												//  3 - file has 3 numbers on a line
			StatisticalErrs();											// Program must calculate statistical errors
			m = Getm();
			ChooseFunction('M',' ');									// M = Single Lorentz Function
		break;
	
		case '2':	// Two Lorentzians on a parabolic background (9 parameters)
			OpenInFile(ParamFile3, sizeof(ParamFile3)/sizeof(char));		// "Double.par"	
			ReadFitParams(stepScale);												
			OpenInFile(DataFile3, sizeof(DataFile3)/sizeof(char));		// "Double.dat"		
			ReadFitData(3);												//  3 - file has 3 numbers on a line
			StatisticalErrs();											// Program must calculate statistical errors
			m = Getm();
			ChooseFunction('N',' ');									// N = Double Lorentz function				
		break;
	}
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
				Marquardt(chiSqr, chiCut, lambda);
		
		trial++;
	}   
	while (fabs(chiOld-chiSqr) >= chiCut); 

	FillYarray();
	chiSqr = CalcChiSq();
2
	switch (fCode)
	{
		case '1':
			FitOut("Lorentz-1 peak.txt", chiSqr, 'M');	//Example 9.1 - parameter errors from Matrix Inversion
		break;
		case '2':
			FitOut("Lorentz-2 peak.txt", chiSqr, 'M');	//Example 9.2 - parameter errors from Matrix Inversion
		break;
	}
}


