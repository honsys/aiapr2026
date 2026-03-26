C Program A.1: \Append-A\SplinTst.For
C Test Cubic Spline Interpolation
C Uses Splines
	Program TestSpline
	Character title(80)
	Real d2a, d2b, xs, x(100), y(100), SplineInt
	Integer n, i
	Open(5,'\f\Append-A\Spline.Dat')      !test data file
	Read(5,1000) title
	Print 1000, ' ',title
 1000 Format(80A1)
	Read(5,*) n, d2a, d2b !no. of points, 2nd derivatives at boundary
	Print *,'Data Table: n=', n
	Print *,'     X                Y'
	do 100 i = 1, n
	  Read(5,*) x(i), y(i)
	  Print *, x(i), y(i)
  100 continue
	Call SplineMake(n, d2a, d2b, x, y)
	Close(5)
  200 Print *,'Type a value of X  (Exit with ctrl C)'
	Read *, xs
	Print *, 'Interpolated y = ', SplineInt(xs)
	goto 200
	END
	Include \f\Append-A\Splines.for

