C Part of Program 11.2: \Chapt-11\LcorProb.For
C Calculate linear correlation probability integral (Table C.3)
C Uses routines in LcorLate.for
	Program LcorProb
	Integer nObserv
	Real LinCorProb, rCorr
	Print *, 'Test Integral of Linear Correlation Function'
 100  Print *, 'Type-# observations, linear correlation coefficient: '
	Read *, nObserv, rCorr
	Print 1000, LinCorProb(nObserv-2, rCorr)
	goto 100
 1000 Format('Integral Correlation Function= ',E10.2,'%')
	END
	Include '\f\Chapt-11\LCorLate.For'
	Include '\f\Chapt-6\FitUtil.For'
