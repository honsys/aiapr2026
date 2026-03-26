//
//  Histograms
//
#include "QuikHist.h"
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;

float histBot, histInt, histTop; 
float histSum, histSum2, histMean, histStDev, histErrInMean;
int   histNbins,histBelow, histIn, histAbove;
int   histBins[100];

// ---------------------------------Define the Histogram, --------------------------------
void HistSetup(float Hbot,float Hint, float Htop)
{
int  i;	
	histBot =Hbot;
	histInt =Hint;
	histTop =Htop;                                       
	histBelow = 0;
	histIn	  = 0;  
	histAbove = 0;
	histSum   = 0;  
	histSum2  = 0;
	histNbins =		(histTop - histBot)/histInt ;
	for (i=1; i == 100; i++) histBins[i] = 0; 
	return;
}


//-------------------------------Put Data in Bins -----------------------------------------
void Histogram(float x)
{
int  k;  
float y;

	y =(x-histBot)/histInt;
	k = (int) y;
	if      (y <  0)            histBelow++;
	else if (k >= histNbins) histAbove++;
	else  //---x is within limits---
	{
		histIn++;
		histSum  = histSum  + x;
		histSum2 = histSum2 + x*x;
		histBins[k]++;
	}		
}

// --------------------------------- Calculate Statistics -----------------------------------
void HistStats()
{
float  hNum;


	hNum = (float) histIn;
	histMean      = histSum/hNum;
    histStDev     = sqrt(histSum2/hNum - (histMean)*(histMean));
	histErrInMean = histStDev/sqrt(hNum);
}

// --------------------------------------- Display --------------------------------------------
void HistDisplay(char title[])
{
int i, j, iscl,  kMax, nMax=50; // kMax is the max number of counts in any bin, nMax is the number of # that can be displayed. 
float lowEdge;
	ofstream out_file2(title);

	if (histIn == 0) 
	{
		cout << "No data in Histogram \n";
		out_file2 << "No data in Histogram \n";
		return;
	}
	cout << setiosflags(ios::fixed |ios::showpoint);
	out_file2 << setiosflags(ios::fixed |ios::showpoint);
	HistStats();
	cout << setprecision(2);
	cout << "Below = " << histBelow <<", In = " << histIn <<", Above = " << histAbove;
	cout << ", Mean= " << histMean << ", Std Dev = "  << histStDev << ", Std Err = " << histErrInMean << endl;
	out_file2 <<title << endl;
	out_file2 << setprecision(2);
	out_file2 << "Below = " << histBelow <<", In = " << histIn <<", Above = " << histAbove;
	out_file2 << ", Mean= " << histMean << ", Std Dev = "  << histStDev << ", Std Err = " << histErrInMean << endl;

    kMax=0;
	for (i=0; i <=  histNbins; i++)
   	{
		if (histBins[i] > kMax) kMax=histBins[i];
	}
    iscl = kMax/nMax +1;
    for (i=0; i <=  histNbins; i++)
    {
		lowEdge = histBot + i*histInt; 
		cout << setw(3) <<lowEdge  << " " <<setw(3) << histBins[i] << " ";
		out_file2 << setw(3) << lowEdge << " " <<setw(3) << histBins[i] << " ";
		if (histBins[i] == 0) 
		{
			cout << endl;
			out_file2 << endl;
		}
		else	
		{
			for (j=1; j<= histBins[i]/iscl; j++)  
			{
				cout << "#";
				out_file2 << "#";
			}
			cout << endl;
			out_file2 << endl;
		} 
	}
	out_file2.close();
	return;
}
