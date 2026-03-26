C Program 7.4: \Chapt-7\Matrix.For
C Invert a square matrix
C Uses FitVars
	Subroutine Matinv(m, mArray, Det)
	Integer m
	Real mArray(10,10), det
	Integer ik(10), jk(10)
	Integer i, j, k, L
	Real aMax, save
        Det=0
C -find largest element
        do 100  k = 1, m
          aMax=0
 1500     do 200  i = k, m
            do 300 j = k , m
              if ( Abs(mArray(i,j)) .GT. Abs(aMax) )  then
                aMax = mArray(i,j)
                ik(k) = i
                jk(k) = j
              endif
  300       continue  ! do j
  200     continue  ! do i
          if (aMax .EQ. 0 )  return  ! with 0 determinant as signal
            Det = 1
C -interchange rows and columns to put aMax in mArray(k,k)
          i = ik(k)
          if (i .LT. k ) then
            goto 1500
	    elseif (i .GT. k )  then
            do 400 j = 1, m
              save = mArray(k,j)
		  mArray(k,j) = mArray(i,j)
              mArray(i,j) = -save
  400       continue                   !  do j
	    endif                        !  if i
          j = jk(k)
          if (j .LT. k ) then
            goto 100
	    elseif (j .GT. k ) then
            do 500 i = 1, m
              save = mArray(i,k)
		  mArray(i,k) = mArray(i,j)
              mArray(i,j) = -save
  500       continue   ! do i
          endIf   ! if j
C -accumulate elements of inverse matrix
          do 600 i = 1, m
		if (i .NE. k )
     1         mArray(i,k) = -mArray(i,k)/aMax
  600     continue ! do i
          do 700 i = 1, m
            do 800 j = 1, m
		  if ((i .NE. k) .and. (j .NE. k) )
     1           mArray(i,j) = mArray(i,j) + mArray(i,k)*mArray(k,j)
  800       continue                   ! do j
  700     continue                     ! do i
	    do 900 j = 1, m
		if (j .NE. k )
     1        mArray(k,j) = mArray(k,j)/aMax
  900     continue                     ! do j
          mArray(k,k) = 1/aMax
          det = det * aMax
  100     continue                     ! do k
C -Restore ordering of matrix
	  do 1000 L = 1, m
          k = m + 1 - L
          j = ik(k)
          if (j .GT. k ) then
		do 1100 i = 1, m
              save = mArray(i,k)
		  mArray(i,k) = -mArray(i,j)
              mArray(i,j) = save
 1100       continue                   ! do i
	    endIf                        ! if j
          i = jk(k)
          if (i .GT. k ) then
		do 1200 j = 1, m
              save = mArray(k,j)
		  mArray(k,j) = -mArray(i,j)
              mArray(i,j) = save
 1200       continue ! do j
	    endIf                        ! if i
 1000   continue                       ! do L
	  Return
	END

	Subroutine LinearBySquare(m, a, b, c)  ! matrix product
	Integer m
	Real a(10), b(10,10), c(10)
	Integer  i,j
        do 100 i = 1, m
          c(i)=0
          do 200 j = 1, m
		c(i)=c(i) +a(j)*b(i,j)
  200     continue
  100   continue
	  Return
	END
