C Program 6.2: \Chapt-6\FitVars.For
C Include file of constants, variables and  arrays for least-squares fits
C -All global Types, Constants and Variables are declared here.
C    The array limits maxData and MaxParam can be set as required
C                                             for particular problems.
	Integer maxData, maxParam, nPts, m, nFree
	Real x, y, sigY, yCalc, a, sigA, beta, alpha,
     1 y0, deltaA, dA, chiSq1, chiSq2, chiSq3, chiOld, chiCut,
     2 stepSize, dYda, grad
	Parameter(maxData=100, maxParam=10)
	Common/FitVars/
     1 nPts, m, nFree, x(maxData), y(maxData), sigY(maxData),
     2 yCalc(maxData), a(maxParam), sigA(maxParam), beta(maxParam),
     3 alpha(maxParam,maxParam),
C
C  -- Variables used by non-linear methods --
     4 y0(maxData), deltaA(maxParam), dA(maxParam), chiSq1, chiSq2,
     5 chiSq3, chiOld, chiCut, stepSize, dYdA(maxData, maxPAram),
     6 grad(maxParam)
C------------------------------
