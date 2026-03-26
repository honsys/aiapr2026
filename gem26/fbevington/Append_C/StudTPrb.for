C Function StudTProb calculates values in Table C.8
C
C  Student's t Probability
	Real Function StudTProb(t)
	Common/student/nu,gam
	Real t, nu
	External StudentsT
	gam       = GammaConst(nu)                  !Calc once for speed
	StudTProb = 2*Simpson(StudentsT, 20, 0, t)  !Prob between +t & -t
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

C Function GammaConst must be called once before Function StudentsT
	Real Function StudentsT(t) !Student's T Distribution (Table C.8)
	Common/student/nu,gam
	Real t, gam, x, nu
C  The following statement is equivalent to
C     x = (1/sqrt(nu*pi)  * (Gamma((nu+1)/2)/Gamma(nu/2))*(1+t^2/nu)^(-(nu+1)/2)
	x  = gam*exp( (-(nu+1)/2)* alog(1+t*t/nu))
	StudentsT = x
	end
	Include C:\f\Append-A\Simpson

