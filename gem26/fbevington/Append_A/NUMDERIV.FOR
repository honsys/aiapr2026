C Program 8.7: \Chapt-8\NUMDERIV.FOR
C Numerical derivatives for non-linear fits
C Can be replaced by analytic derivatives, if they can be calculated.
C However, numerical calculation is general, and convenient.
C Uses FitFunc8
	Real Function dXiSq_da(j)
	Integer j
	Common/numderiv/ xiSq0         !must be preserved for reuse
	Real    CalcChiSq, xiSqPlus, xiSq0, tem
	Include '\f\Chapt-6\FitVars.For'
	  if (j.EQ.1 )  xiSq0 = CalcChiSq()  !  starting point-calculate it once
        a(j)        =  a(j) + deltaA(j)
	  xiSqPlus    =  CalcChiSq()
	  a(j)        =  a(j) - deltaA(j)    ! restore
	  tem         =  (xiSqPlus - xiSq0)/(deltaA(j))
	  dXiSq_da    =  tem
	  Return
	END

	Real Function d2XiSq_da2(j)
	Integer j
	Integer i
	Real    yFunction, tem
	Include '\f\Chapt-6\FitVars.For'
	  if (j .EQ. 1 ) then
	    do 100 i = 1, nPts
		y0(i) = yFunction(x(i))       ! starting point-calculate it once
  100     continue
	  endIf
        a(j) = a(j) + deltaA(j)
        tem = 0
	  do 200 i = 1, nPts
	    dYda(i,j) = (yFunction(x(i)) - y0(i))/deltaA(j)/sigY(i)
	    tem = tem + (dYda(i,j))**2
  200   continue
        a(j) = a(j) - deltaA(j)
	  d2XiSq_da2 = 2*tem
	  Return
	END

	Real Function d2XiSq_dajk(j,k)
	Integer j,k
	Real tem
	Integer i
	Include '\f\Chapt-6\FitVars.For'
	  tem = 0
        do 300 i = 1, nPts
          tem = tem + dYda(i,j)*dYda(i,k)
  300   continue
        d2XiSq_dajk = 2*tem
	  Return
	END
