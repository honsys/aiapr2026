C Program 6.3: \Chapt-6\FitUtil.For
C Utility routines for fitting programs
	Real Function ChiX(x) !External routine, called by Simpson as FunctX
	Common/util/ glSimps
	Real glSimps
	Real x
	  if (x.EQ.0) then
	    ChiX = 0.0
	  else
	    ChiX = x**(glSimps-1)*exp(-x/2)  ! glSimps = h = nFree/2
	  endIf
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

	Real Function ChiProbDens(x,nFree)
	Real num, den, h, x
	Integer nFree
	  h = nFree/2.0
	  num = x**(h-1) * exp(-x/2)
	  den =  2**h * gamma(h)
	  ChiProbDens = num/den
	  Return
	END

	Subroutine ErrorMatrix    ! list the error matrix
	Include '\f\Chapt-6\FitVars.For'
	Integer j,k
	  Write (6,*)
	  Write (6,*) 'Error matrix - Alpha'
        do 100 j = 1,m
	    write(6, 1000) (alpha(j,k), k = 1,m)
	    write(6,*)
  100   continue
 1000   Format(6E12.4)
	  Return
	END

	Subroutine FetchData(fName, title)
	Character*(*) fName, title
	Include '\f\Chapt-6\FitVars.For'
	Integer i
	  Open(5, fName)
	  Read(5, 1000) title
 1000 Format(A80)
	  Read(5, *) nPts
	  do 100 i = 1, nPts
	    read(5, *) x(i), y(i), sigY(i)
	    if (SigY(i) .EQ. 0 ) sigY(i) = sqrt(Y(i)) ! Poisson uncertainty
C         if (sigY(i) .EQ. 0 ) sigY(i) = 1
  100   continue
	Close(5)
	Return
	END

	Subroutine FetchParameters
	Include '\f\Chapt-6\FitVars.For'
	Integer  i, m1
	  Print *, 'Type number of parameters  (or type -8, -9 for Ch 8,9
     1sample data) '
	  Read *, m1
	  if (m1 .GT. 0) then
	    m = m1
	    Print *, 'Type starting values for ',m, ' parameters: '
	    Read *,  (a(i), i = 1, m)
	  elseIf (m1 .EQ. -8) then
		m = 5                       ! for Chapter 8
		a(1) = 10.0
		a(2) = 900.0
		a(3) = 80.0
		a(4) = 27.0
		a(5) = 225.0
	  elseIf (m1 .EQ. -9) then
		m = 6                       ! for Chapter 9
		a(1) =   1.0
		a(2) =   1.0
		a(3) =   1.0
		a(4) =   1.0
		a(5) =   1.0
		a(6) =   0.1
	  endIf
	  nFree = nPts - m
	  if (nFree .LE. 0 )
     1          Call ErrorAbort('Less than 1 degree of freedom.')
	  do 100 i = 1, m
  100     deltaA(i) = stepSize*abs(a(i)) ! Implies a(i) must not be 0,
	  Return
	END

	Subroutine OutPut(errMatrix, outDev, chi2, title)
	Logical errMatrix
	Character*(*) title, outDev
	Real chi2
	Integer i
	Real chiProb, c2PerDof
	Include '\f\Chapt-6\FitVars.For'
	Open(unit=6, file=outDev)          ! allow output to printer or file
	  nFree = nPts-m
	  c2PerDof = chi2/nFree
	  chiPrb = 100*chiProb(nFree, chi2)
	  Write(6, *)
	  Write(6, 1000)  title
 1000   Format(1xA40)
	  Write(6, 1010)  chi2, nFree, c2PerDof,chiPrb
 1010   Format(' Chi2Sq = ', F7.2, ' for', I3, ' deg of freedom,',
     1   ' chiSq/dof = ',F5.3,/,', ChiSq probability = ',F5.1, '%')
	  Write(6, 1020) m
 1020   Format(' Fitted parameters: a(1) - a[',I2,']')
	  Write(6, 1030) (a(i), i=1,m)
 1030   Format(8F10.4)
	  Write(6, 1040) m
 1040   Format(' Uncertainties: sigmaA(1) - sigmaA[',I2,']')
	  Write(6, 1030) (sigA(i), i=1,m)
	  If (errMatrix ) Call ErrorMatrix
	  Print *
	  Print *,'Hit Enter to continue.'
	  Read *
	  Write(6, 1050)
 1050   Format('   pt #    X             Y         dY      yCalc ')
	  do 200 i = 1,nPts
	    Write(6,1060) i, x(i), y(i), sigY(i), yCalc(i)
 1060   Format(I5, F10.3, F12.3, F10.3, F10.3)
  200   continue
	  Print *, 'Hit Enter to continue.'
	  Close(6)
	  Return
	END


C Approximate Gamma Function with h = nFree/2
	Real Function Gamma(h)
	Real  h,  pi/3.1415927/
	  Gamma = sqrt(2.0*pi) * exp(-h)*(h**(h-0.5)) * (1.0 + 0.0833/h)
	  Return
	End

C -Simpson's rule for "Functx(x:real):real"
C  If Functx has other parameters, they must be global, e.g., glSimps
C
	Real Function Simpson(Functx, nInts, loLim, hiLim) ! 2 calcs/interval
	External functX ! this statement req'd in calling pgm also
	Real  FunctX, sum, x, dx, loLim, hiLim
	Integer  nInts, i
	  x = loLim
	  dx = (hiLim - loLim)/(2*nInts)
	  sum=Functx(x)
	  sum= sum - FunctX(hiLim)
	 do 100 i = 1, nInts
          x=x+2*dx
	    sum=sum + 4*FunctX(x-dx) + 2*FunctX(x)
  100     continue
        sum = sum
	  Simpson = sum*dx/3.0
	  Return
	End

	Character Function ReadChar() ! returns upper case
	Character t
	Integer iTem
	  Read *, t
	  iTem = iChar(t)
	  if (iTem .Gt. 96) iTem = iTem - 32
	  ReadChar = Char(iTem)
	  Return
	End

	Logical Function Ask(s)
	Character*(*)  s
	Character  readChar
	  Print *, s, ' Y or N '
	  Ask = (readChar() .EQ. 'Y')
	  Return
	End

	Subroutine ErrorAbort(s)
	  Character*(*) s
	  Print *, s
	  Call Exit
	  Return
	End

