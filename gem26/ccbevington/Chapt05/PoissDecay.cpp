//Program 5.2: Simulated decay of an unstable state.
// Program PoisDcay;  { Generate a 200-event Poisson  histogram }
// Uses  CRT, QuikHist, Montelib;
// This program write two files: out_file1  "PoissDecay-times.txt",  a list of Poisson-distributed decay times;       
//							and  out_file2  "PoissDecay-summary.txt, the statistics and histogram of the decay times.

#include "c:\Cpp\CppLibrary\MonteLib.h"
#include "c:\cpp\CppLibrary\QuikHist.h"
#include "fstream.h"
#include <iomanip.h>

ofstream out_file1("PoissDecay-times.txt");// open file for list of decay times 

void main()
{
float lo = 0., dx = 1., hi = 22.;
float mu = 8.4,  nEvents = 200.,  nbins = 10.;
float x;

char test;

int   i ,k;
unsigned	
	s1 = 1171,   //seeds for random number generation
	s2 = 343,
	s3 = 1322;
	SetRandomDeviateSeed(s1, s2, s3);
	HistSetup(lo,dx,hi);
	k = PoissonDeviate(mu,true);//Initialize - make the table, nothing returned
	out_file1 <<"Poisson Deviates\n";
	out_file1 << "Seeds at beginning = " << s1  << "  " << s2 <<"  " <<s3 <<endl;
	for (i=1; i<=nEvents; i++)
		{
		k = PoissonDeviate(mu,false);
		x = (float) k;
		out_file1  << setw(8) <<  i 	
  				   << setw(8) <<  k    << endl;
		Histogram(x);
	}
	GetRandomDeviateSeed(s1, s2, s3);
	HistDisplay("Poisson_Decay.hst");	
	cout << "Seeds at end = " << s1  << "  " << s2 <<"  " <<s3 <<endl;
	ofstream out_file2("PoissDecay-summary.txt",ios::app);
	out_file1 << "Seeds at end = " << s1  << "  " << s2 <<"  " <<s3 <<endl;
	out_file2 << "Seeds at end = " << s1  << "  " << s2 <<"  " <<s3 <<endl;
	out_file1.close();
	out_file2.close();
}
