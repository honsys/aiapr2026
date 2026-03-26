C program StudentsT (Function StudentsT calculates values in Table C.8)
C Calculates both the Gaussian and the Student's t probability for exceeding
C  a given value of t=(mu-x)/sigma, where mu is the mean value of x and
C sigma is the uncertainty in the mean.
C For speed, and to reduce possibility of overflow, we don't make 2 calls
C  to Function Gamma, but calculate the ratio directly in Function GammaConst.
C
	program StudT
	Common/student/nu,gam
	Real gP, tP, t, nu
  100 print  *,'Type nDOF and t = |mu-x|/sigma. Terminate with ctrl C '
	read *, n, t
	nu = n
	call gTProb(gP, tP, t)
	print 1100, 100*tP, 100*(1-tP)
	print 1200, 100*gP, 100*(1-gP)
	print *
	goto 100
 1100 format(' Prob (Student''s t) = ',f6.1, '%, 1-Prob = ',F5.2,'%')
 1200 format(' Prob (Gaussian)    = ' ,f6.1, '%, 1-Prob = ',F5.2,'%')
	end

	Real Function StudentsT(t) !Student's T Distribution (Table C.8)
	Common/student/nu,gam
	Real t, gam, x, nu
C  The following statement is equivalent to
C     x = (1/sqrt(nu*pi)  * (Gamma((nu+1)/2)/Gamma(nu/2))*(1+t^2/nu)^(-(nu+1)/2)
	x  = gam*exp( (-(nu+1)/2)* alog(1+t*t/nu))
	StudentsT = x
	end

	Real Function Gauss(x)
	Real pi/3.14159/,x
	Gauss = Exp(-x*x/2)/sqrt(2*pi)
	return
	end

C  Compare Gaussian and Student's t Probabilities
	Subroutine gtProb(gProb, tProb, t)
	Common/student/nu,gam
	Real gProb, tprob,t, nu
	External StudentsT, Gauss
	gam      = GammaConst(nu)                   !Calc once for speed
	tProb =  2*Simpson(StudentsT, 20, 0, t)     !Prob between +t & -t
	gProb =  2*Simpson(Gauss, 20, 0, t)
	return
	end

	Real Function GammaConst(h)
	Real pi/3.14159/
	Real h, y1, y2, G
C   G  = Gamma((h+1)/2)/Gamma(h/2)/sqrt(h*pi)
C   Pre-calculate for speed and to  avoid overflow
	y1 = -0.5*(h+1) + 0.5*(h)  *alog(0.5*(h+1))
	y2 = -0.5*h     + 0.5*(h-1)*alog(0.5*h)
	G  = exp(y1-y2)*(1+0.0833/(0.5*(h+1)))/((1+0.0833/(0.5*h))
	1  *sqrt(h*pi))
	GammaConst = G
	return
	END
	Include c:\f\Chapt-6\Fitutil.for            !  for Simpson

