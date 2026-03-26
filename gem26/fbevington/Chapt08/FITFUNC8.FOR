C Program 8.5:  \Chapt-8\FITFUNC8.FOR
C Uses FitVars
C -The following routines are general for fitting any function-
	Subroutine CalculateY
	Real Yfunction
	Include '\f\Chapt-6\FitVars.For'
	  do 100 i = 1, nPts
          yCalc(i) = Yfunction(x(i))
  100   continue
	  Return
	END

	Real Function CalcChiSq()
	Real  chi2, Yfunction
	Include '\f\Chapt-6\FitVars.For'
	  chi2=0.
	  do 100 i = 1, nPts
	    chi2 = chi2 + ( (y(i)-Yfunction(x(i)))/sigY(i))**2
  100   continue
        CalcChiSq = chi2
	  Return
	END

C -Standard deviation calc'd from chiSq change of 1 (parabola fit)
	Real Function  SigParab(j)
	Integer j
	Real CalcChiSq
	Include '\f\Chapt-6\FitVars.For'
	  chiSq2 = CalcChiSq()
        a(j)   = a(j) + deltaA(j)
	  chiSq3 = CalcChiSq()
        a(j)   = a(j) - 2*deltaA(j)
	  chiSq1 = CalcChiSq()
        a(j)   = a(j) + deltaA(j)
        SigParab = deltaA(j)*sqrt(2/(chiSq1-2*chiSq2+chiSq3))
	  Return
	END

C -Standard deviation calc'd from diagonal terms in error matrix
	Real Function SigMatrx(j)
	Integer j
	Real sig
	Include '\f\Chapt-6\FitVars.For'
	  sig = sqrt(abs(alpha(j,j)))
        if (alpha(j,j) .LT. 0 ) sig = - sig  ! note- an error
        SigMatrx = sig
	  Return
	END
