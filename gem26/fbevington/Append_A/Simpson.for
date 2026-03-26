C Function Simpson is included in \Chapt-6\FitUtil.for
C Simpson's rule for "Functx(x:real):real"
C If Functx has other parameters, they must be global, e.g., glSimps
C
	Real Function Simpson(Functx, nInts, loLim, hiLim) ! 2 calcs/interval
	External functX ! this statement req'd in calling pgm also
	Real  FunctX, sum, x, dx, loLim, hiLim
	Integer  nInts, i
	x = loLim
	dx = (hiLim - loLim)/(2*nInts)
	sum=Functx(x) - FunctX(hiLim) !subtract at hiLim to avoid double counting
	do 100 i = 1, nInts
	  x=x+2*dx
	  sum=sum + 4*FunctX(x-dx) + 2*FunctX(x)
  100 continue
	sum = sum
	Simpson = sum*dx/3.0
	Return
	End

