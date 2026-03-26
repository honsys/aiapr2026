C Program 5.3: \Chapt-5\MonteLib.For
C Monte Carlo library routines
C Uses GenUtil

	Subroutine SetRandomDeviateSeed(sa,sb,sc)
	Include '\F\Chapt-5\MonteInc.For'
	Integer sa, sb, sc
	  seed1 = sa
	  seed2 = sb
	  seed3 = sc
	  Return
	END

	Subroutine GetRandomDeviateSeed(sa,sb,sc)
	Include '\F\Chapt-5\MonteInc.For'
	Integer sa, sb, sc
	  sa = seed1
	  sb = seed2
	  sc = seed3
	  Return
	END

	Real Function RandomDeviate()            !  Wichmann and Hill
	Include '\F\Chapt-5\MonteInc.For'
	Real  temp
	  seed1 = 171*Mod(seed1,177) - 2*(seed1 / 177)
          if (seed1 .LT. 0 ) seed1 = seed1 + 30269
	  seed2 = 172*Mod(seed2,176) - 35*(seed2 / 176)
          if (seed2 .LT. 0 ) seed2 = seed2 + 30307
	  seed3 = 170*Mod(seed3,178) - 63*(seed3 / 178)
          if (seed3 .LT. 0 ) seed3 = seed3 + 30323
	  temp = seed1/30269. + seed2/30307. + seed3/30323.
	  RandomDeviate = temp-Aint(temp)
	  Return
	END

C -Find a random variable drawn from the Gaussian distribution-
	Real Function RandomGaussDeviate()           ! Box-Mueller
	Include '\F\Chapt-5\MonteInc.For'
	Logical  nextVar/.false./
	Real r, f, z1, z2, x1RanGauss, RandomDeviate
	  if (nextVar) then
          nextVar = .false.
          RandomGaussDeviate = X2RanGauss
	  else
 100      z1 = -1 + 2*RandomDeviate()
	    z2 = -1 + 2*RandomDeviate()
	    r  = z1*z1 + z2*z2
	    if (r .GE. 1) goto 100
	    f = sqrt(-2*Alog(r)/r)
          X1RanGauss = z1*f
          X2RanGauss = z2*f
          RandomGaussDeviate = X1RanGauss
	    nextVar = .true.
	  endIf
	  Return
	END

	Real Function GaussSmear(x,dx)
	Real x, dx
	Real RandomGAussDeviate
	  GaussSmear = x + RandomGaussDeviate() * dx
	  Return
	END

C -Recursion method for Poisson Probability (P(n,m). To find P(n,m) must
C    all with successive arguments j=0,1,..n. Max mu=85, no limit on x
	Real Function PoissonRecur(j, m)
	Include '\F\Chapt-5\MonteInc.For'
	Integer j
	Real m
	  if (j.EQ.0 ) then
	    poiss = exp(-m)
	  else
	    poiss = (poiss*m)/j ! poiss =  (m^j)exp(-mu/j!)
        endIf
	  PoissonRecur = poiss
	  Return
	END

C -Find a random variable drawn from the Poisson distribution
	Integer Function PoissonDeviate(mu, init)
	Include '\F\Chapt-5\MonteInc.for'
	Integer   i, x, n
	Real      mu, p , r, PoissonRecur
	Logical   init
	  if (init ) then               ! --- Make table of sums ---
	    n = Aint(mu + 8* sqrt(mu))  ! ie., 8*sigma
	    if (n .GT. maxBins ) then
		print *,' Overflow error in routine PoissonDeviate'
		call exit
	    end if
	    Ptable(0) = PoissonRecur(0,mu)
	    do 100 i = 1, n-1
            p = PoissonRecur(i,mu)
		Ptable(i) = Ptable(i-1)+p
  100     continue
	    Ptable(n) = 1               ! --- Assure unit probability ---
	  else                          ! --- Generate an event ---
          x = -1
	    r = RandomDeviate()
  200     x = 1 + x
	    if (Ptable(x) .LE. r) goto 200
          PoissonDeviate = x
	  endIf
	  Return
	END