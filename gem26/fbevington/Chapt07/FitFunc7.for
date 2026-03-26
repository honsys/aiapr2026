C Program 7.2: \Chapt-7\FitFunc7.For
C Fitting functions for Chapter 7 examples.
	Real Function PowerFunc(k, xx)
	Integer k
	Real    xx
	Real    yy
	Integer i
	  yy = 1
        if (k .GT. 1 ) then
          do 100 i= 2, k
		yy = xx * yy
  100     continue
        endIf
	  PowerFunc = yy
	  Return
	END

	Real Function LegFunc(k, xx)
C -Define separate terms in a series, y = a0*L0(x) + a1*L1(x) + ..
C  Note k = 1 corresponds to zeroth order.
C var   PAE : char    'P'-power series,
C                     'A'-all Legendre terms to order m,
C                     'E'-even Legendre terms}
	Common /FitVars7/PAE
	Character * 1 PAE
	Integer k
	Real    xx
	Integer kk, i
	Real  c, pi/3.14159/, LegPoly(11) ! i.e., 0th thru 10th order
	  if (PAE .EQ. 'E') kk = 2*k - 1
	  if (PAE .EQ.'A')  kk = k
	  c = cos(pi*xx/180)
        LegPoly(1) = 1 ! for better efficiency, could calc once and save
        if (kk .GT. 1 ) then
          LegPoly(2) = c
          if (kk .GT. 2 ) then
            do 100 i = 3, kk
              LegPoly(i)=((2*i-1)*c*LegPoly(i-1)-(i-1)*LegPoly(i-2))/i
  100       continue
	    endIf        ! kk > 2
	  endIf          ! kk > 1
        LegFunc = LegPoly(kk)
	  Return
	END

	Real Function Funct(k, xx)
	Integer k
	Real xx
	Real LegFunc, PowerFunc
	Common /FitVars7/PAE
	Character * 1 PAE
	  if ((PAE .EQ. 'A') .OR. (PAE.EQ.'E')) Funct = LegFunc(k,xx)
	  if  (PAE .EQ. 'P')  Funct = PowerFunc(k,xx)
	  Return
	END

	Subroutine CalculateY
	Integer  i, k
	Real     yy, Funct
	Include '\f\Chapt-6\FitVars.For'
	  do 100 i=1, nPts
	    yy = 0
          do 200 k = 1, m
		yy = yy + a(k) * Funct(k,x(i))
  200     continue
	    yCalc(i) = yy
  100   continue
	  Return
	END

	Real Function CalcChiSq() ! assumes array yCalc has been filled
      integer i
      real    chi2
	Include '\f\Chapt-6\FitVars.For'
        chi2=0.
        do 100 i = 1, nPts
	    chi2 = chi2 + ( (y(i)-yCalc(i)) / sigY(i))**2
  100   continue
        CalcChiSq = chi2
	  Return
	END
