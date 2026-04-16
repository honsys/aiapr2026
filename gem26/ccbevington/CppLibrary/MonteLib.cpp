//Program 5.3: Monte Carlo library routines.
//Unit MonteLib;

#include "MonteLib.h"

const  int maxBins = 100;
long  seed1, seed2, seed3;
float x2RanGauss, poiss,ptable[maxBins];


void SetRandomDeviateSeed(unsigned sa, unsigned sb,unsigned sc)
{
	seed1 = sa;  
	seed2 = sb;  
	seed3 = sc;
};

void GetRandomDeviateSeed(unsigned &sa, unsigned &sb, unsigned &sc)
{
	sa = seed1;
	sb = seed2;  
	sc = seed3;
	};

float RandomDeviate()
{   
	float  temp, temDev;
	seed1 = 171*(seed1 % 177) - 2*(seed1 / 177);
    if (seed1 < 0)  
		seed1 = seed1 + 30269;
	seed2 = 172*(seed2 % 176) - 35*(seed2 / 176);
    if (seed2 < 0) 
		seed2 = seed2 + 30307;
	seed3 = 170*(seed3 % 178) - 63*(seed3 / 178);
    if (seed3 < 0) 
		seed3 = seed3 + 30323;
	temp = seed1/30269. + seed2/30307. + seed3/30323.;
	temDev = temp-int(temp);
	return temDev;
};


float RandomGaussDeviate()		//--- Box-Mueller - find a random variable drawn from the Gaussian distribution
{
	bool nextVar = false;
	float  r, f, z1, z2, x1RanGauss, rgd;
    if (nextVar == true) 
	{
		nextVar = false;
		rgd = x2RanGauss;
	}
    else
	{
		do                 //repeat
		{
			z1 = -1 + 2*RandomDeviate();
			z2 = -1 + 2*RandomDeviate();
			r  = z1*z1 + z2*z2;
		}
		while (r >= 1);							//--- until r < 1;
		f = sqrt(-2*log(r)/r);
		x1RanGauss = z1*f;
		x2RanGauss = z2*f;
		rgd = x1RanGauss;
		nextVar = true;
	};
	return rgd; 
};

float GaussSmear(float x, float dx)
{
	float gs;
	gs =  x + RandomGaussDeviate() * dx;
	return gs;
};

float PoissonRecur(int j, float m)				//--- Recursion method for Poisson Probability (P(n;m). 
{												//    To find P(n;m), call with successive arguments j=0,1,..n.
	if (j==0)									// 	   max mu=85, no limit on x 
		poiss = exp(-m);
	else 
	   poiss = poiss*m/j;						//--- poiss =  (m^j)exp(-mu/j!) 
	return poiss;
};

int PoissonDeviate(float mu, bool init)			//--- Find a random variable drawn from the Poisson distribution

{
	int  i,jx,n, jpd = 0;
	float  p,r;
	if (init) 
	{											//--- Make table of sums ---
		n = int(mu + 8* sqrt(mu));   //i.e., 8*sigma
		if (n > maxBins)
		{
			printf("Overflow err in Poisson dev");//--- ErrorAbort('Overflow err in Poisson dev')
			exit(1);
		}
		ptable[0] = PoissonRecur(0,mu);
		for (i=1;  i <= n-1; i++)				//--- for i = 1 to n-1 do
		{
			p = PoissonRecur(i,mu);
			ptable[i] = ptable[i-1]+p;
		};
  		ptable[n] = 1;							//--- Assure unit probability ---
	} 
	else
	{											//--- Generate an event ---
		jx = -1;
		r = RandomDeviate();
		do										//--- repeat
			jx++																															;
		while (ptable[jx] <= r);
		jpd = jx;                                //--- PoissonDeviate = x;
	};
	return jpd; 
};

