C Program 7.1: \Chapt-7\MultRegr.For
C Least-squares fit to a power series and to Legendre polynomials.
C Uses  FitFunc7, MakeAB7, Matrix, FitUtil, GenUtil
	Program MultRegr
C m = num of parameters, nPts=number of data pairs,
C Data and uncertainties are in arrays  x, y, dy.
	Include '\f\Chapt-6\FitVars.For'
	Common /FitVars7/PAE
	Character * 1 PAE
	Real     det, chi2, CalcChiSq
	Integer  i
	Logical  spl
	Character*1 ReadChar
	Character*40 title
	Print *,    '(P)ower series, (A)ll Legendre terms to L = 4,'
	Print *,    'or (E)ven Legendre terms(L = 0,2,4).'
	Print *,    'Type P,A or E '
	PAE = ReadChar()
 1000 Format(A1)
	if (PAE .EQ. 'P') then
	  Call FetchData ('\f\Chapt-7\ThermCou.Dat', title)
	  Print *,    'Type number of parameters '
	  Read *, m
	elseIf (PAE .EQ. 'A') then
	  Call FetchData('\f\Chapt-7\Legendre.Dat',title)
	  m = 5
	elseIf (PAE .EQ. 'E' ) then
	  Call FetchData('\f\Chapt-7\Legendre.Dat',title)
	  m = 3
	endIf  !PAE
	Call MakeBeta                        ! set up the linear beta matrix
	Call MakeAlpha                       ! set up the square alpha matrix
	Call Matinv(m, Alpha, det)           ! invert alph to get beta matrix
	Call LinearBySquare(m,Beta,Alpha,a)  ! beta x epsilon.EQ.parameter matrix
	Call CalculateY
	chi2 = CalcChiSq()
	do 100 i = 1, m
	  sigA(i) = sqrt(alpha(i,i))
  100 continue
	Call OutPut(.TRUE., 'CON', chi2, Title) ! TRUE to print error matrix
	if (m .GT. 2 ) then
	  spl = .TRUE.                       ! plot a curve
	else
	  spl = .FALSE.                      ! plot a line
	endIf
	if (PAE .EQ. 'P') then
	  Call PlotIt('MultRegr.SCR', .FALSE., spl,    !file,log?,spline
     1  C', (x(2)-x(1))/12,   !data Circles, radius of data cir
     2  -10., -2., 110., 4.,   ! x1,y1, x2,y2
     3  6, 6,                 ! x,y grid marks
     4  'Temperature (degrees Celsius)','Voltage (mV)')
	else if ((PAE .EQ. 'A') .OR. (PAE .EQ. 'E')) then
	  Call PlotIt('MultRegr.SCR', .FALSE., .TRUE.,
     1  C', (x(2)-x(1))/10,   0., 0.,    180., 1500., 6, 6,
     2  'Theta(degrees)', 'Number of Counts')
	endIF                                ! PAE
	Call CloseGraphics
	END
	Include '\f\Chapt-7\FitFunc7.For'
	Include '\f\Chapt-7\MakeAB7.For'
	Include '\f\Append-B\Matrix.For'
	Include '\f\Chapt-6\FitUtil.For'

