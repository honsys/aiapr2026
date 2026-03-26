C Program 8.0: \Chapt-8\NonLinFt.For
C Main calling routine for non-linear fitting methods
C Uses GridSear, GradSear, ExpndFit, MarqFit, FitFunc8, MakeAB8,
C                                     NumDeriv, Matrix, FitUtil
	Program NonLinFt
	Integer  trial, j, method
	Real stepDown, lambda, chiSqr, CalcChiSq
	Character*40 title
	Real stepScale(4)/0.49999, 0.99999, 0.001, 0.001/
	Include '\f\Chapt-6\FitVars.For'
	Print *,'     (1)Grid Search,     (2)Gradient Search'
	Print *,'     (3)ChiSq expansion, (4)Function expansion'
	Print *, 'Type 1, 2, 3, or 4  --- '
	Read *, method
	chiCut    = 0.01
	stepDown  = 0.1                ! step down the gradient in Gradls
	lambda    = 0.001              ! for Marquardt method only
	stepSize  = stepScale(method)  ! scales deltaA(j)
	Call FetchData('\f\Chapt-8\RadioDk.hst',title)
	Call FetchParameters      ! parameters are in this routine in \FitUtil
	Close(5)
	trial     = 0
	chiSqr    = CalcChisq()
	chiOld = chiSqr + chiCut + 1
	do while (abs(chiOld - chiSqr) .GE. chiCut)
	  chiOld = chiSqr
	  Print 1000, trial, chiSqr
 1000   Format(' Trial #', I4, ' chiSq =', F10.1)
	  Print 1100, (a(j), j = 1,m)
 1100   Format(6F12.4)
	  Print *
	  goto (110, 120, 130, 140) , method
  110     Call Gridls(chiSqr)
	    goto 150   ! WAS 151
  120     Call Gradls(chiSqr, stepDown)
	    goto 150
  130     Call ChiFit(chiSqr)
	    goto 150
  140     Call Marquardt(chiSqr, chiCut, lambda)
  150   trial = trial +1
	endDo
  151 Call CalculateY
	if ((method .EQ. 1) .OR. (method .EQ. 2)) then
	  do 200 j = 1, m
	    SigA(j) = SigParab(j)  ! dChi2 = 1
  200   continue
	  Call OutPut(.FALSE., 'CON' ,chiSqr, title) ! no error matrix
	elseIf ((method .EQ. 3) .OR. (method .EQ. 4)) then
	  if (method .EQ.  4  ) then
	    Call Marquardt(chiSqr,chiCut,0)    ! get error matrix
	  endIf
	  do 300 j = 1, m
	    SigA(j) = SigMatrx(j) ! error matrix
  300   continue
	  Call OutPut(.TRUE., 'CON', chiSqr, title) ! with error matrix
	endIf
	END

C Sample fitting function for non-linear fits
C Example is sum of 2 exponentials on a constant background
	Real Function  ExpF(a,x)
	Real a,x
	Real yy, arg
	arg = abs(x/a)
	if (arg .GT. 60 ) then
	  yy = 0
	else
	  yy = Exp(-arg)
	endIf
	ExpF = yy
	Return
	END

	Function  yFunction(xx) ! real
	Real yFunction, xx, ExpF
	Include '\f\Chapt-6\FitVars.For'
	Yfunction = a(1) + a(2)*ExpF(a(4),xx) + a(3)*ExpF(a(5),xx)
	Return
	END

	Include '\f\Chapt-8\GridSear.For'      ! 1-Grid search method
	Include '\f\Chapt-8\GradSear.For'      ! 2-Gradient search method
	Include '\f\Chapt-8\ExpndFit.For'      ! 3-Function expansion method
	Include '\f\Chapt-8\MarqFit.For'       ! 4-Marquardt method
	Include '\f\Chapt-6\FitUtil.For'
	Include '\f\Chapt-8\FitFunc8.For'      ! Used by all methods
	Include '\f\Chapt-8\MakeAB8.For'       ! Used by methods 4 and 5
	Include '\f\Append-A\NumDeriv.For'     ! Used by methods 4 and 5
	Include '\f\Append-B\Matrix.For'       ! Used by methods 4 and 5



