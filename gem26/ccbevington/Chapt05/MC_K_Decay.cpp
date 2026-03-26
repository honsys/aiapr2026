//Program GenKdecay;  //Monte Carlo for K-decay

#include "c:\cpp\chapt_5\MonteLib.h"
#include "c:\cpp\Append_D\QuikHist.h"
#include "fstream.h"
#include <iomanip.h>
#include "c:\cpp\Chapt_6\GenUtil.h"


 
 double pcm, beta, gamma, t;
 unsigned s1, s2, s3;
 int i,j;

//--------------for K-Decay------10-23-01
const double 
	tauKaon	 = 0.894E-10,
	massKaon = 497.7,
	d1		 =  10.0, //Fiducial in cm
	d2		 =  40.0,
	xMean	 =  5.00,
	xSig	 =  0.5,
	pMean	 =  2000.,
	pSig	 =  100.,
	c		 =  3.00E10;
//----------------------------------------

double EfromP(double p, double m)
{
	double y;
	y=sqrt(p*p+m*m);
	return(y);
};

double PfromE(double e, double m)
{
	double y;
	y=sqrt(e*e-m*m);
	return(y);
};

double GammaFromBeta(double b)
{
	double y;
	y = 1/sqrt(1-b*b);
	return(y);
};

double PLorentz(double p, double m, double cosTheta,double beta, double gamma)
    //enter with pcm and theta, return Plong in Lab
{
	double y;
	y=gamma*(p*cosTheta+beta*EfromP(p,m));
	return(y);
};

double Atan(double y,double x)
{
	double th;
	th=atan(y/x);			//????????????????
	if (x < 0.)  th=th+PI;
	if ((x > 0.) && (y < 0)) th=th+2*PI;
	return(th);
};


double LifeTimeCM(double tau)
{
	double r, y;

	r = RandomDeviate();
	y = -tau*log(1-r);
	return(y);
};

    //---------{ main program---------- K-decay in flight 10-23-01 ------
void main()
{
	double timeCM, timeLab, range;
	int countTotal, countGood, totalRequest;
	double x0, tau, decayLoc, p, e;

	cout<< "Type total number of good events to be generated ";
	cin >> totalRequest;
	ofstream out_file1("MC_K-decay_data.txt");

	cout      << "Monte Carlo Generation for example 5.7" << endl;
	cout      << "Type total good events to generate " <<totalRequest << massKaon << d1 << d2 << tauKaon <<  "(nEvents, k-mass, Fiducial- d1, d2, mass kaon)"<< endl;
	out_file1 << "Monte Carlo Generation for example 5.7" << endl;
	out_file1 << "Request " << totalRequest << "  Mkaon= " << massKaon << ",low cut= "<<  d1 
		                 << ", hi cut= " << d2 << ", tau-K= " << tauKaon << endl;
	HistSetup(0., tauKaon*1E10/20.1, tauKaon*1E10);
	SetRandomDeviateSeed(943, 919, 3113);
	countTotal = 0;
	countGood=0;
	do
	{
		x0       = RandomGaussDeviate()*xSig+xMean;   //production point
		p        = RandomGaussDeviate()*pSig+pMean;   //kaon momentum
		beta     = p/EfromP(p,massKaon);            //kaon velocity/c
		gamma    = GammaFromBeta(beta);
		timeCM   = LifeTimeCM(tauKaon);             //lifetime in kaon frame
		timeLab  = timeCM*gamma;                    //Lorents xfor to lab
		range    = timeLab*beta*c;                  //kaon range
		decayLoc = x0+range;                        //kaon decay point
		i++;
		countTotal++;
		out_file1.precision(4);
		cout.precision(4);
		if ((decayLoc >= d1) && (decayLoc < d2))
		{
			countGood++;
			out_file1 
				<< countGood 
				<< "     " 
				<< x0 
				<<"      " 
				<< p 
				<<"      "
				<< decayLoc 
				<<"      "  
				<< timeCM*1e10 
				<< endl;
			Histogram(timeCM*1E10);
		}
	}
	while (countGood < totalRequest);

 		GetRandomDeviateSeed(s1, s2, s3);
		HistDisplay("MC_K-decay_summary.txt");	
		cout << "Seeds at end = " << s1  << "  " << s2 <<"  " <<s3 << endl;
		cout <<"Output files: " << "MC_K-Decay_summary.txt and " <<"MC_K_Decay-data.txt" << endl;
		out_file1 <<-1 << endl;
		out_file1 <<" total number generated = " << countTotal <<" total successes = " << countGood << endl;
		out_file1 << "Seeds at end = " << s1  << "  " << s2 <<"  " <<s3 <<endl;
		out_file1.close();
}

