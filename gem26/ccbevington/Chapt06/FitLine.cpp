//	Program 6.1: Least-squares fit to a straight line by the method of determinants
//	Program FitLine - Least-squares fit to Y=a[1]+a[2]*X by determinants
//  uses FitVars, FitUtil, GenUtil;  

#include <fstream.h>
#include <math.h>
#include <stdlib.h>
#include "FitUtil-ch6.h"
#include "c:\cpp\CppLibrary\GenUtil.h"


//       void CalculateY();          //fills array Ycalc
//       double CalcChiSq();			//assumes array yCalc has been filled
//       double LineFit();			//returns determinant of successful, otherwise -1;



char sBuffer[] = "                                                                          ";
int iiBuffer[30];
double ddBuffer[30];
int i, j, nInts, nData, nDbls;


//--------------------------Data Input -----------------------

void ReadFitData()
{		ReadString(sBuffer, sizeof(sBuffer)/sizeof(char));
		PutTitle(sBuffer);
		nInts = 1;
		ReadIntegers(nInts, iiBuffer, sizeof(iiBuffer)/sizeof(int));
		nData = iiBuffer[1];
		nDbls = 3;
		PutTitle(sBuffer);
		PutnPts(nData);
		for (j = 1;j<= nData; j++)
		{
			ReadDoubles(nDbls, ddBuffer, sizeof(ddBuffer)/sizeof(double));
			PutXYdY(j, ddBuffer[1], ddBuffer[2], ddBuffer[3]);
		}		
}


//--------------------------MAIN  ROUTINE-----------------------

void main()
{

	char    VorG;
	int	m = 2;										//find 2 parameters
	double chi2;														
	char FileName1[] = "Volts.dat";
	char FileName2[] = "Geiger.dat";

	// --------------------- Select data set
	cout << "(v)olts or ( g)eiger? ";
	cin >> VorG;
	switch (VorG)
	{
		case  'v':	//------------------------------------------------------------// Example 6.1		
		OpenInFile(FileName1, sizeof(FileName1)/sizeof(char));			
		ReadFitData();
		Putm(m);
		//ChooseFunction('P',' ');//-----------------------------------------------// Power series
		break;

		case 'g':  //------------------------------------------------------------// Example 6.2
		OpenInFile(FileName2, sizeof(FileName2)/sizeof(char));			
		ReadFitData();
		Putm(m);
		InvertX();												//fitting with  1/r^2 as independent variable
		StatisticalErrs();
		//ChooseFunction('P',' ');								// Power series
		break;
	}
		if (LineFit() < 0)	//----------------------- Fit a straight line to the data by determinants										
		{
			ErrorMessage("Determinant is 0 or negative");
			return;
		};
		m = 2;
		chi2 = CalcChiSq();
	switch (VorG)
	{
		case 'v':														
			FitOut("Volts.txt", chi2, 'D');						// 	Example 6.1';   - Determinant fit,	
		break;
		case 'g':
			FitOut("Geiger.txt", chi2, 'D');					//  Example 6.2		- Determinant fit,
		break;
	}
return;
}
