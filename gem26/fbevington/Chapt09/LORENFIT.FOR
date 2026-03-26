C Program 9.1: \Chapt-9\LorenFit.For
C Main calling routine for fit to Lorentzian + polynomial
C Uses  FitFunc8, MarqFit, Matrix,  NumDeriv, MakeAB8,  FitUtil,
C                                      QuikPlot, GenUtil
	Program LorenFit
	Character*40  title
	Integer trial, j
	Real   xShift, chiSqr, lambda, YFunction
	Real  stepScale(4)/ 0.49999, 0.99999, 0.001, 0.001/
	Include 'C:\f\Chapt-6\FitVars.for'
	chiCut    = 0.01
	lambda    = 0.001             ! for Marquardt method only
	stepSize  = stepScale(4)      ! scales deltaA[j]
	Call FetchData('\f\Chapt-9\Single.Hst',title)
 	xShift = (x(2)- x(1))/2
	do  j = 1, nPts
          	x(j) = x(j) + xShift        ! move to bin center
	endDo
	Call FetchParameters    ! parameters are in this routine in \FitUtil
	trial     = 0
	chiSqr    = CalcChisq()
	chiOld = chiSqr + chiCut +1
	do while (abs(chiOld - chiSqr) .GT. chiCut)
          	chiOld = chiSqr
		Print *,'Trial #',trial,' chiSq = ',chiSqr
		Print *, (a(j), j = 1, m)
		Call Marquardt(chiSqr, chiCut, lambda)
		trial  = 1 + trial
	endDo
	Call CalculateY
	Call Marquardt(chiSqr,chiCut,0)          ! get error matrix
	do j = 1, m
		SigA(j) = SigMatrx(j)                  ! error matrix
	endDo
	Call OutPut(.TRUE., 'CON', chiSqr,title) ! with error matrix
	do j = 1, nPts
		x(j) = x(j) - xShift                   ! restore to left edge
	endDo
	END

C Lorentzian peak on a quadratic background
	Real Function yFunction(xx) ! Lorentzian on polynomial
	Real xx
	Real yy, pi/3.1415927/
	Include '\f\Chapt-6\FitVars.For'
	yy = a(1) + a(2)*xx + a(3)*xx**2 + a(4)*a(6)/(2*pi)
     1   /((xx-a(5))**2 + a(6)**2/4)
	yFunction = yy
	Return
	END
	Include '\f\Chapt-6\FitUtil.For'
	Include '\f\Chapt-8\FitFunc8.For'
	Include '\f\Chapt-8\MarqFit.For'       	! Marquardt method
	Include '\f\Chapt-8\MakeAB8.For'       	! Used by MarqFit
	Include '\f\Append-A\NumDeriv.For'         ! Used by MarqFit
	Include '\f\Append-B\Matrix.For'          ! Used by MarqFit





