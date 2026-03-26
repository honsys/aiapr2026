// Program 7.1: Least-squares fit by matrices to a power series and to Legendre polynomials.
//Program MultRegr;
#include <fstream.h>
#include <math.h>
#include "FitUtil-ch7.h"
#include "c:\cpp\CppLibrary\GenUtil.h"

// m=num of parameters 
// nPts=number of data pairs,
// arrays  x,y,dy are data and errors.            }


char	sBuffer[] = "                                                                         ";
int		iiBuffer[30];
double	ddBuffer[30];
int		i, j, nInts, nData, nDbls;
double	det, chi2;    

void ReadFitData()
{
	ReadString(sBuffer, sizeof(sBuffer)/sizeof(char));
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

void main()
{
	char FileName1[] = "ThermCoupleE7-2.Dat";
	char FileName2[] = "LegendreAngleE7-3.Dat";
	char  pae;
	int m;

	cout << "(p)ower series, (a)ll Legendre terms to L=4 or (e)ven Legendre terms(L=0,2,4)" << endl;
	cout <<"Type p,a or e  ";
	cin >>  pae;
	switch (pae)
		{
			case 'p':	// Polynomial
				cout << "Type number of parameters ";
				cin  >>   m;
				ChooseFunction('P',' ');
				OpenInFile(FileName1, sizeof(FileName1)/sizeof(char));			
				ReadFitData();									// Read file "ThermCouple-E7-2.Dat"
				Putm(m);

				break;

			case 'a':											// All orders of Legendre Polynomial
				ChooseFunction('L','a');
				OpenInFile(FileName2, sizeof(FileName2)/sizeof(char));			
				ReadFitData();									//Read file "LegendreAngleE7-3.Dat"
				Putm(5);
				StatisticalErrs();
			break;

			case  'e':													
				ChooseFunction('L','e');
				OpenInFile(FileName2, sizeof(FileName2)/sizeof(char));			
				ReadFitData();									// Read file "LegendreAngleE7-3.Dat"
				Putm(3);
				StatisticalErrs();
			break;
		}
	MakeBeta();      //set up the linear beta matrix
	MakeAlpha();     //set up the square alpha matrix
	det = MatInv(); //invert alph to get beta matrix

	if (det = 0) 
	return;

	SquareByRow(); //beta x epsilon=parameter matrix
	SigMatrix();
	FillYarray();
	chi2 = CalcChiSq();

switch (pae)
	{
		case 'p':														//'M' for error matrix
			FitOut("ThermCouple.txt", chi2, 'M');						//Example 6.1
		break;
		case 'a':
			FitOut("LegendreAngleAll.txt", chi2,  'M');					//Example 6.2
			break;
		case 'e':
			FitOut("LegendreAngleEven.txt", chi2,  'M');				//Example 6.2
		break;

	}
	
}	