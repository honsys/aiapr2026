C Function Chi1Prob.for -  part of Program 11.1   (Table C.1)
C Calculate chi^2 prob. dens. & the chi^2 prob. integral
C These routines are also in library \f\chapt-6\FitUtil.for     Program Chi2Prob
	Real Function ChiProbDens(x,nFree) !not used by chi2Prob. See chiX.
	Common/util/glSimps
	Real glSimps
	Real num, den, h, x
	Integer nFree
	  h = nFree/2.0
	  num = x**(h-1) * exp(-x/2)
	  den =  2**h * gamma(h)
	  ChiProbDens = num/den
	  Return
	END

	Real Function ChiProb(nFree, chi2) ! max nFree =  56
	External ChiX
	Common/util/ glSimps
	Real ChiX, Simpson, glSimps
	Integer nFree
	Real pi, chi2, cLim, intFromLim
	Data    cLim /2/,                  ! expansion limit for nFree = 1
     1        intFromLim /0.157/,        ! integral from cLim to infinity
     2        dx0 /0.2/                  ! determines accuracy of integration
     3        pi/3.14159/
	Integer nInt
	  if (chi2 .GE. 1) then
	    nInt = (chi2+0.0001)/dx0
	  else
	    nInt = 5
	  endIf
	  if (chi2 .GT. 15*sqrt(nFree) ) then   ! quick cutout
	  ChiProb = 0
	  else
	    glSimps = float(nFree)/2            ! glSimps is global for ChiX
	    if (nFree .EQ. 1 )    then
		if (chi2 .LT. cLim ) then
		  ChiProb = 1-sqrt(chi2/2/pi)*
     1        (2 - chi2*(1/3 - chi2*(1/20 - chi2*(1/168 - chi2/1728))))
		else
		  ChiProb = intFromLim - Simpson(ChiX,nInt,cLim,chi2)
     1                              /Gamma(nFree/2.0)/2.0**(nFree/2.0)
		endIf                             ! if (chi2 ...)
	    else if (nFree .EQ. 2 ) then

		ChiProb = exp(-chi2/2)            ! Integrable
	    else
		ChiProb =  1 - Simpson(ChiX, nInt, 0, chi2)
     1                            /Gamma(nFree/2.0)/2.0**(nFree/2.0)
	    endIf                               ! if (nFree ...)
	  endIf
	  Return                                ! if (nFree ...)
	END

	Real Function ChiX(x) !External routine, called by Simpson as FunctX
C     ChiX is numerator of Function ChiProbDens.  Constant denominator is
C         calculated once after integration.
	Common/util/glSimps  ! glSimps = h = nFree/2
	Real glSimps
	Real x
	  if (x.EQ.0) then
	    ChiX = 0.0
	  else
	    ChiX = x**(glSimps-1)*exp(-x/2)
	  endIf
	  Return
	END

C Approximate Gamma Function with h = nFree/2
	Real Function Gamma(h)
	Real  h,  pi/3.1415927/
	  Gamma = sqrt(2.0*pi) * exp(-h)*(h**(h-0.5)) * (1.0 + 0.0833/h)
	  Return
	End
	Include c:\f\append-A\Simpson.for

