C Program 8.3: \Chapt-8\EXPNDFIT.FOR
C Non-linear Least-squares fit by expansion of the fitting function
C Uses  FitFunc8, MakeAB8, Matrix
	Subroutine ChiFit(chiSqr)
	Integer j
	Real det, CalcChiSq
	Include '\f\Chapt-6\FitVars.For'
	  Call MakeBeta
	  Call MakeAlpha
	  Call MatInv(m, alpha, det)            ! Invert matrix
	  Call LinearBySquare(m,Beta,Alpha,dA)  ! Evalulate parameter increments
	  do 100  j = 1, m
	    a(j) = a(j) + dA(j)                 ! Increment to next solution.
  100   continue
	  Print *,'a',(a(j),j=1,m)
	  chiSqr =  CalcChiSq()
	  Return
	END
