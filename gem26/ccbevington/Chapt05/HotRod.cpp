//HotRod     Temperature variation along a rod - Example 5.4

#include "c:\Cpp\CppLibrary\MonteLib.h"
#include "fstream.h"
#include <iomanip.h>

void main()
{
int  i, n=10;					//generate 10 points at 1 cm intervals   
float x,t,tPrime,sigmaT=1.0; //with an uncertainty of +-1 degree
unsigned 
	s1 = 1171,
	s2 = 343,
	s3 = 1322;
	SetRandomDeviateSeed(s1, s2, s3);
//  clrScr;
	cout	<< setiosflags(ios::showpoint | ios::fixed) 
			<<	" Hot Rod Test Data" << endl;
	cout << "  i      x          T    T' (smeared)" 
		 << ";  sigmaT =  " << setw(10) << setprecision(2) << sigmaT << endl;
	x = -0.5;
	for (i=1; i<= n;  i++)
//position along rod
	{                  
		x = x+1;
		t  = 10*x;           //calculate mean temperature at point
		tPrime= GaussSmear(t,sigmaT);   //smear it
		cout	<< setprecision(0)	<< setw(3)	<< i
				<< setprecision(2)	<< setw(8)	<<	x
				<< setprecision(2)	<<setw(10)	<<	t				
									<<setw(10)	<<	tPrime 
				<<	endl;
	}
	GetRandomDeviateSeed(s1, s2, s3);
	cout << "Final seeds " << setprecision(0)  << s1 << "  " << s2 << "  " << s3 << endl;
  return;
}
