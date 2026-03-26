
C Program 6.1: \Chapt-6\FitLine.For
C Least-squares fit to a straight line by method of determinants
C Uses  FitUtil, GenUtil
C  --------------------------MAIN  ROUTINE-----------------------
	Program FitLine
	Include '\f\Chapt-6\FitVars.For'
	Character*40  title
	Character*1 VorG, ReadChar
	Integer i
	Real det, chi2, CalcChiSq
	m = 2                                           ! find 2 parameters
	Print *, '(V)olts or (G)eiger? '
	VorG = ReadChar()
	if (VorG .EQ. 'V') then
	  Call FetchData('\f\Chapt-6\Volts.Dat' ,title)    ! Example 6.1
	elseIf ((VorG .EQ. 'G') .OR. (VorG .EQ. 'g')) then
	  Call FetchData('\f\Chapt-6\Geiger.Dat',title)    ! Example 6.2
	  do 100 i = 1 , nPts
	    x(i)  = 1/x(i)**2                            ! fitting 1/r^2
  100   continue
	endIf
	Call LineFit(det)
	Call CalculateY           ! fill array Ycalc for CalcChiSq and PlotIt
	chi2 = CalcChiSq()
	Call OutPut(.FALSE. , 'CON', chi2, title)  !FALSE for no error matrix
	if (VorG .EQ. 'V') then
	  Call PlotIt('\f\FitLine.SCR',.FALSE.,.FALSE.,!scrpt file,log?,spline?
     1        'C', abs(x(2)-x(1))/20,              !data Circle, rad of Circle
     2         0.0, 0.0, 100.0, 3.0,               ! x1,y1,x2,y2
     3         5, 6,                               ! # x-div, # y-div
     4        'X (cm)', 'Potential Diff(erence (Volts)')  ! axis labels
	elseIF (VorG .EQ. 'G') then
	  Call PlotIt('\f\FitLine.SCR',.FALSE. ,.FALSE.,
     1         'C', abs(x(2)-x(1))/50,  0.0, 0.0, 30.0, 1000.0,  6, 5,
     2  'Squared Inverse Distance (1/m^2)', 'Number of Counts per sec')
	endIf
	read *
	Call CloseGraphics
	END

	Subroutine CalculateY          ! fills array Ycalc
	Include '\f\Chapt-6\FitVars.For'
	Integer i
	do 100 i= 1 , nPts
	  yCalc(i) = a(1) + a(2)*x(i)
  100 continue
	Return
	END

	Real Function CalcChiSq() ! assumes array yCalc has been filled
	Include '\f\Chapt-6\FitVars.For'
	Integer  i
	Real     chi2
	chi2=0.
	do 100 i = 1 , nPts
	  chi2 = chi2 + ( (y(i)-yCalc(i))/sigY(i))**2
  100 continue
	CalcChiSq = chi2
	Return
	END

	Subroutine LineFit(det)
	Include '\f\Chapt-6\FitVars.For'
	Real det
	Integer i
	Real sumWt, sumX, sumY, sumX2, sumY2, sumXY, weight
	sumWt = 0
	sumX  = 0
	sumY  = 0
	sumX2 = 0
	sumY2 = 0
	sumXY = 0
C  --------- accumulate weighted sums -----------
	do 100  i= 1 , nPts
	  weight  = 1/sigY(i)**2
	  sumWt   = sumWt  + weight
	  sumX    = sumX   + weight * x(i)
	  sumY    = sumY   + weight * y(i)
	  sumX2   = sumX2  + weight * x(i)**2
	  sumY2   = sumY2  + weight * y(i)**2
	  sumXY   = sumXY  + weight * x(i)*y(i)
  100 continue
C ---calculate the parameters - cut out if determinant is not > 0 ---
	det =  sumWt * sumX2 - sumX * sumX
	if (det .GT. 0 )  then
	  a(1)     = (sumx2*sumY  - sumX*sumXY)/det
	  a(2)     = (sumXY*sumWt - sumX*sumY) /det
	  sigA(1)  = sqrt(sumX2/det)
	  sigA(2)  = sqrt(sumWt/det)
	else
	  Call ErrorAbort('Determinant < or = 0 in LineFit')
	endIF
	Return
	END
	Include '\f\Chapt-6\FitUtil.For

