C Program 11.1: \f\Chapt-11\TestChi2.For -  part of Program 11.1   (Table C.1)
C Calculate chi^2 prob. dens. & the chi^2 prob. integral
C Uses ChiProbDens and ChiProb and in Chapter 6, FitUtil.for
      Program Chi2Prob
	Real chi2,  ChiProb
	Integer nFree
100   Print *
	Print *,'Calculate Chi2 probability density function & integral',
     1 ' probability'
	Print *, 'Type  Num deg of freedom and  Chi2  (exit on "Ctrl c")'
	Print *
	Read *,  nFree, Chi2
	Print 1000, ChiProbDens(Chi2, nFree), 100*ChiProb(nFree, chi2)
 1000 format(' Chi^2 prob. dens. = ',f7.3,' , Chi^2 Probability=',
     1f7.3,'%')
	Print *
	Print *,' ***** Note that Table C.4 refers to Chi^2/nFree****'
	goto 100
	END
	Include C:\f\append-c\chi2prob.for
