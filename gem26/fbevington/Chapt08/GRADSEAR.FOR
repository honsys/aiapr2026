C Program 8.2: \Chapt-8\GRADSEAR.FOR
C Non-linear least-squares fit by gradient search method
C Uses FitFunc8, FitUtil
	Subroutine CalcGrad
	Integer j
	Real   sum, deltA, fract/0.001/, CalcChiSq
	Include '\f\Chapt-6\FitVars.For'
	  sum = 0
	  do j = 1, m
	    chiSq2 = CalcChiSq()
	    deltA  = fract * deltaA(j) ! diff(erential element for gradent
	    a(j)    = a(j) + deltA
	    chiSq1  = CalcChiSq()
	    a(j)    = a(j) - deltA
	    grad(j) = chiSq2 - ChiSq1                  ! 2*deltA*grad
	    sum     = sum + grad(j)**2
	   endDo
	  do  j = 1, m
	    grad(j) =  deltaA(j)*grad(j)/sqrt(sum)     ! step * grad
	  endDo
	  Return
	End

	Subroutine Gradls(chiSqr, stepDown)
	Real chiSqr, stepDown
	Real stepSum, step1, CalcChiSq
	Integer j
	Include '\f\Chapt-6\FitVars.For'
	Call CalcGrad   ! calculate the gradient
C -Evaluate chiSqr at new point and make sure chiSqr decreases-
	chiSq3 = chiSq2 + 1
	do while (chiSq3 .gt. chiSq2)
	  do j = 1, m
	    a(j) = a(j) + stepDown * grad(j) ! slide down
	  endDo
	  chiSq3 = CalcChiSq()
	  if (chiSq3 .ge. chiSq2 ) then
	    do j = 1, m      !  must have overshot minimum
	      a(j) = a(j) - stepDown * grad(j) ! restore
  	    endDo 	
	    stepDown = stepDown/2              ! decrease stepSize
	  endIf
	endDo
	stepSum = 0
C -Increment parameters until chiSqr starts to increase-
	do while (chiSq3 .lt. chiSq2)
	  stepSum = stepSum + stepDown   ! counts total increment
	  chiSq1 = chiSq2
	  chiSq2 = chiSq3
	  do j = 1, m
	    a(j) = a(j) + stepDown * grad(j)
	  endDo
	  chiSq3 = CalcChiSq()
	endDo                 !doWhile
C -Find minimum of parabola defined by last three points-
	step1=stepDown*((chiSq3-chiSq2)/(chiSq1-2*chiSq2+chiSq3)+0.5)
	do j = 1, m
	  a(j) = a(j) - step1 * grad(j)    ! move to minimum
	endDo
	chiSqr = CalcChiSq()
	stepDown = stepSum                 ! start with this next time
	Return
	END
