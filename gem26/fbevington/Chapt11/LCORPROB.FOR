C Program 11.2: \Chapt-11\LcorProb.For
C Calculate linear correlation probability integral
C Uses routines in LcorLate.for
	Program LcorProb
	Integer nObserv
	Real LinCorProb, rCorr
	Print *, 'Test Integral of Linear Correlation Function'
	Print *, 'Type-# observations, linear correlation coefficient: '
	Read *, nObserv, rCorr
	Print 1000, LinCorProb(nObserv-2, rCorr)
 1000 Format('Integral Correlation Function= ',E10.2,'%')
C      Print *, 'Integral Correlation Function= ',
C     1 LinCorProb(nObserv-2, rCorr),'%'
      END
	Include '\f\Chapt-11\LCorLate.For'
	Include '\f\Chapt-6\FitUtil.For'
