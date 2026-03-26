C Program A.2: \Append-A\Splines.For
C Routines for cubic spline interpolation.
C Constant intervals in the independent variable are assumed.
	Subroutine SplineMake(nn, d2ydx2A, d2ydx2B, xIn, yIn)
	Integer nn
	Real    d2ydx2A, d2ydx2B, xIn(100), yIn(100)
C -Common variables set in SplineMake, used in SplineInt-
	Common/Splines/n, h, xx(100), yy(100), d2ydx2(100)
	Integer n
	Real h, xx, yy, d2ydx2
	Integer i
	Real  a(100), delt1(100), delt2(100), b(100)
	  n = nn         ! Makes n available to subroutine SplineInt     ????
	  h = (xIn(n) - xIn(1))/(n-1)
	  do 100 i = 1, n
	    xx(i) = xIn(i)
	    yy(i) = yIn(i)
  100   continue
	  d2ydx2(1) = d2ydx2A ! end values of 2nd derivatives provided on input
	  d2ydx2(n) = d2ydx2B
	  a(2) = 4
	  do 200 i =  3, n-1
	    a(i) = 4-1/a(i-1)              ! coefficients
  200   continue
	  do 300 i = 2, n
	    delt1(i) = yIn(i) - yIn(i-1)   ! 1st differences
  300   continue
	  do 400 i = 2, n-1                ! 2nd differences x 6
	    delt2(i) = 6*(delt1(i+1) - delt1(i))/(h*h)
  400   continue
	  b(2) = delt2(2) - d2ydx2(1)      ! b coefficients
	  do 500 i= 3, n-1
	    b(i) = delt2(i) - b(i-1)/a(i-1)
  500   continue
	  b(n-1) = b(n-1) - d2ydx2(n)
	  d2ydx2(n-1) = b(n-1)/a(n-1)
	  do 600 i = n-2, 2, -1
	    d2ydx2(i) = (b(i) - d2ydx2(i+1))/a(i) ! 2nd derivatives
  600   continue
	  Return
	END

	Real Function DyDx(i) ! first derivative
	Integer i
	Common/Splines/n, h, xx(100), yy(100), d2ydx2(100)
	Integer n
	Real h, xx, yy, d2ydx2
	  DyDx = (yy(i+1)-yy(i))/h - h*(d2ydx2(i)/3+d2ydx2(i+1)/6)
	  Return
	END

	Real Function D3yDx3(i) ! third derivative
	Integer i
	Common/Splines/n, h, xx(100), yy(100), d2ydx2(100)
	Integer n
	Real h, xx, yy, d2ydx2
	  D3yDx3 = (d2ydx2(i+1) - d2ydx2(i))/h
	  Return
	END

	Real Function SplineInt(x) !Interpolate in table made by SplineMake
	Real x
	Common/Splines/n, h, xx(100), yy(100), d2ydx2(100)
	Integer n
	Real h, xx, yy, d2ydx2, DyDx, D3yDx3, dx
	Integer i
	  i = Int((x-xx(1))/h)+1
	  if (i .LT. 1 )   i = 1
	  if (i .GT. n-1 ) i = n-1
	  dx = x -xx(i)
C -interpolate x-
	  if (i .EQ. n ) then
	    SplineInt = yy(i)
	  else
	    SplineInt = yy(i) + (DyDx(i) + (d2ydx2(i)/2 +
     1                                          D3yDx3(i)/6*dx)*dx)*dx
	  endIf
	  Return
	END
