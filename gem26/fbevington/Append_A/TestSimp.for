C Test Simpson's Rule to Integrate y=x^2 from x = 0 to 10.
	Program TestSimp
	External Squared
	yIntegral = Simpson(Squared,5,0.,10.)
	print *,'Integral of x**2 from 0 to 10 =', yIntegral
	END

C Test function  !Simpson's integral is exact for a quadratic function
	Real Function Squared(x)
	Squared = x**2
	Return
	END
	Include c:\f\Append-A\Simpson.for


