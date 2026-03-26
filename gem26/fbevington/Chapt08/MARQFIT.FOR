C Program 8.4: \Chapt-8\MarqFit.For
C Non-linear fit by the gradient-expansion (Marquardt) method
C Uses  FitFunc9, MakeAB8, Matrix
	Subroutine Marquardt(chiSqr, xiCut, lambda)
	Integer j
	Real chiSqr, xiCut, lambda
	Real det, CalcChiSq
	Include '\f\Chapt-6\FitVars.For'
	do
	   Call MakeBeta
	   Call MakeAlpha
	   do 100 j = 1, m
		alpha(j,j) = (1 + lambda) * alpha(j,j)
  100    continue
	   Call MatInv(m, alpha, det)  ! Invert matrix
	   if (lambda .LE. 0 ) Return ! Final call to get the error matrix.
	   Call LinearBySquare(m,Beta,Alpha,dA) ! Evaluate parameter increments
	   chiSq1 = chiSqr
	   do 200 j = 1, m
		a(j) = a(j) + dA(j) ! Incr to next solution
  200    continue
	   chiSqr = CalcChiSq()
	   if ( chiSqr .LE. chiSq1 + xiCut ) return
	   do 300 j = 1, m
		a(j) = a(j)-dA(j) ! Return to prev solution
  300    continue
	   chiSqr = CalcChiSq()
	   lambda = 10*lambda  ! and repeat the calc, with larger lambda
	end do
	END
