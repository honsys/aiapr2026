C Program 8.6: \Chapt-8\MakeAB8.For
C Matrix set-up for non-linear fits
C Uses Fitfunc9, NumDeriv
C
	Subroutine MakeBeta       ! Make beta matrices for non-linear fitting
	Integer  j
	Include '\f\Chapt-6\FitVars.For'
	  do 100 j = 1, m
          beta(j) = -0.5*dXiSq_da(j)
100     continue
	  Return
	END

	Subroutine MakeAlpha  ! Make alpha matrices for non-linear fitting
	Integer  j, k
	Include '\f\Chapt-6\FitVars.For'
	  do 100 j = 1, m
          alpha(j,j) =  0.5 * d2XiSq_da2(j)
          if (alpha(j,j) .EQ. 0 ) then
		Print *, 'Diagonal element is zero, j =',j
		Stop
          endIf
          if (j .GT. 1 ) then
            do 200 k = 1, j-1
              alpha(j,k) =  0.5*d2XiSq_dajk(j,k)
              alpha(k,j) =  alpha(j,k)
  200       continue   ! do k
	    endIf        ! if  j
  100   continue       !  do j
        do 300 j = 1, m
          if (alpha(j,j) .LT. 0 ) then
		alpha(j,j) = -alpha(j,j)
		if (j .GT. 1 ) then
              do 400 k = 1, j-1
                alpha(j,k) = 0
                alpha(k,j) = 0
  400         continue ! do k
		endIf      ! if j
	    endIf        ! if alpha
  300   continue       ! for j
	  Return
	END
