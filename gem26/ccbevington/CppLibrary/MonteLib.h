//
//  MonteLib.h
//
#include <iostream>
#include <math.h>

void SetRandomDeviateSeed(unsigned sa, unsigned sb, unsigned  sc);
void GetRandomDeviateSeed(unsigned &sa,unsigned &sb, unsigned &sc);
float RandomGaussDeviate();
float RandomDeviate();
float GaussSmear(float x, float dx);
float PoissonRecur(int j, float m);
int PoissonDeviate(float mu, bool init);
