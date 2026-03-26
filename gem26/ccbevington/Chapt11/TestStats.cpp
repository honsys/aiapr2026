#include <iomanip.h>
#include "StatFunctions.h"


// =================================== Test  Routines ==========================================

main()
{
	char	select;
	double	x,y,h;
	cout << "Type the lead letter to test:" << endl<< endl;
	cout <<	"(g)amma function approximation for integers and half iuntegers" << endl;
	cout << "or the  following probabilities: "<< endl; 
	cout <<	"(G)aussian, (L)inear correlation, (C)hi^2, (F)-test or, (S)tudent's t ";
	cin >> select;
	cout << endl;
	switch	(select)
	{
// -------------------------------------- Test Gamma Function ----------------------------------	
		case 'g':		
		cout << "Test Gamma finction" << endl;
			cout << "Type x ";
			cin >> x;
			cout <<"Gamma("<< x << ") = " << Gamma(x);
		break;
	
// --------------------------------------------------Gaussian -----------------------------------------
		case 'G':
			cout << "TEST Gaussain Probability" << endl;
			cout << "Type  x ";
			cin >> x;
			y = GaussProb( x);
			cout <<  "Gaussian Probability = " << 100* y << '%' << endl;
		break;
// ------------------------------------- Test Linear Correlation --------------------------------
		case 'L':
			int numObs;
			cout << "TEST Linear Correlation" << endl;
			cout << "Type Linear Correlation Coefficient and  NumObs ";
			cin >> x >> numObs;
			y =  LinCorProb(x, numObs-2);
			cout << "Integral correlation function = " << y << endl;
		break;

// ---------------------------------------Test Chi^2 --------------------------------------------
		case 'C':
			cout << "TEST Chi^2 Probability" << endl;
			cout << " Type chi^2 per dof, and the number of degrees of freedom  ";
			cin  >> x >> h;
			y = ChiProb(h, h*x);
			cout <<" Prob of obtaining a value of chi^2/dof > "<< x << " = " << 100*y<<"%"  << endl;
		break;

// ---------------------------------------------- F Distribution --------------------------------------
		case 'F':
			double f, nu1, nu2;
			
			cout << "Test F Probability" << endl;
			cout << "Type  f, nu1, nu2 ";
			cin >>f  >> nu1  >> nu2;
			y = F_Prob( f, nu1, nu2);
			cout <<  "f= " << f << " f-Prob= " << y << endl;
		break;

//----------------------------------------------- Student's t ------------------------------------	
		case 'S':
			cout << "TEST Student's t Probability" << endl;
			cout << "Type t (|mu - x|/sigma) and the number of degrees of freedom ";
			cin >> x >> h;
			y = Stud_tProb(x, h);
			cout << "deg of freedom, t= " <<h << ' ' << x <<" Stud_t prob = " << 100*y <<"%, << (100-prob) = " << 100*(1 - y)<<'%';
			cout << endl;
		break;
	}

//-------------------------------------------------------------------------------------------------	
	return(0);

}




