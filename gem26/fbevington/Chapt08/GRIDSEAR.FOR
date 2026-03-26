C  Program 8.1: \chapt-8\GRIDSEAR.FOR
C Non-linear fit by the grid-search method
C Uses FitFunc8, FitUtil
	Subroutine Gridls(chiSqr)
	Real chiSqr
	Real CalcChiSq
	Real save,delta, delta1,del1,del2,aa,bb,cc,disc,alph,x1,x2
	Integer j
	Include '\f\Chapt-6\FitVars.For'
	  chiSq2 = CalcChiSq()
C  -find local minimum for each parameter-
	  do 100 j = 1, m
	    delta    = deltaA(j)
          a(j)     = a(j) + delta
	    chiSq3   = CalcChiSq()
          if (chiSq3 .GT. chiSq2 ) then
		delta  = -delta                ! started in wrong direction
		a(j)   =  a(j) + delta
		save   =  chiSq2               ! interchange 2 and 3 so 3 is lower
		chiSq2 =  chiSq3
            chiSq3 =  save
          endIf
C  -Increment or decrement a(j) until chi squared increases-
  110     continue
		chiSq1 = chiSq2                ! move back to prepare for quad fit
            chiSq2 = chiSq3
		a(j)   = a(j) + delta
		chiSq3 = CalcChiSq()
          if (chiSq3 .LE.  chiSq2) goto 110
C  -Find minimum of parabola defined by last three points-
	    del1 = chiSq2 - chiSq1
          del2 = chiSq3 - 2*chiSq2 + chiSq1
          delta1 = delta * (del1/del2 + 1.5)
          a(j) = a(j)  - delta1
	    chiSq2 = CalcChiSq()             ! at new local minimum
C  -Adjust delta for change of 2 from chiSq at minimum-
	    aa = del2/2                      ! chiSq = aa*a(j)**2 + bb*a(j) + cc
          bb = del1 - del2/2
          cc = chiSq1-chiSq2
	    disc = bb**2 -4*aa*(cc-2)        ! chiSqr diff(erence) = 2
	    if (disc .GT. 0 ) then      ! if not, then probably not parabolic yet
		disc =sqrt(disc)
		alph = (-bb - disc)/(2*aa)
		x1 = alph*delta +  a(1) - 2*delta   ! a(j) at chiSq minimum+2
		disc = bb**2 - 4*aa*cc
		if (disc.GT.0 ) then
		  disc = sqrt(disc)
		else
		  disc = 0 ! elim rounding err
		endIf
		alph = (-bb - disc)/(2*aa)
		x2 = alph*delta + a(1) - 2*delta    ! a(j) at chiSq minimum
            delta = x1 - x2
            deltaA(j) = delta
	    endIf
  100   continue                                ! do j
	  chiSqr = chiSq2
	  Return
	END
