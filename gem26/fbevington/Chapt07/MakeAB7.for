C Program 7.3: \Chapt-7\MakeAB7.For
C Routines to set up the beta and alpha matrices for linear regression
C Uses  Matrix, FitFunc7
C
	Subroutine MakeBeta               ! Make the beta matrices
	Integer  i, k
	Real Funct
	Include 'C:\f\Chapt-6\FitVars.For'
	  do 100  k=1, m
          Beta(k)=0
          do 200 i=1, nPts
		Beta(k)=Beta(k) + y(i)*Funct(k, x(i))/sigY(i)**2
  200     continue
  100   continue
	  Return
	End

	Subroutine MakeAlpha              ! Make the alpha matrices
	Integer i,j,k
	Real Funct
	Include 'C:\f\Chapt-6\FitVars.For'
	  do 100 j=1, m
          do 200 k=1, m
            Alpha(j,k)=0
            do 300 i=1, nPts
              Alpha(j,k) = Alpha(j,k)+Funct(j, x(i))
     1                                *Funct(k, x(i))/sigY(i)**2
  300       continue
  200     continue
  100   continue
	  Return
	End

