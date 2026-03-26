C  Gaussian Probability - integral probability of obtaining a value
C   which is within z standard deviations to the mean.
	Function GaussProb(z)
	External GaussDens
	real z
	integer n
	n = 20
	y = 2*Simpson(GaussDens,n, 0, z)
	GaussProb = y
	END
c
c  Gaussian Probability Density
	Real Function GaussDens(x)
	real x
	y = 1/sqrt(2*3.14159)*exp(-x**2/2)
	GaussDens = y
	Return
	END


