C Part of Program 11.2: \Chapt-11\LCorrlate.For
C Function and integral for  Linear-correlation probability
C Uses FitUtil, GenUtil
	Real Function LinCorProb(nFree, hiLim)
	External LinCorrel     ! for use in Function Simpson
      Integer nFree
	Real    hiLim
	Real dx /0.01/, loLim/0.0/, LinCorrel, Simpson
      Integer  nInt
      Common/Util/glSimps
	  glSimps = nFree ! global for Function LinCorrel when called by Simpson
	  nInt   = Int((hiLim - loLim)/dx)
	  LinCorProb = 1-2*Simpson(LinCorrel, nInt, loLim,  hiLim)
	  Return
	END

	Real Function LinCorrel(r)
      Real r
	Common/Util/glSimps        ! glSims = nFree must be global for
	Data sqrtPi/1.7724539/     ! Function "Simpsons" which allows only 1 arg
        LinCorrel = Gamma((glSimps+1)/2)/Gamma(glSimps/2)
     1            *exp( (glSimps-2)/2 * aLog(1 - r**2))/sqrtPi
	  Return
	END
