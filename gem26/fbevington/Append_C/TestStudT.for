C Program calculates both the Gaussian and the Student's t probability
C of exceeding a given value of t=(mu-x)/sigma, where
C mu is the mean value of x and
C sigma is the uncertainty in the mean.
C Functions StudTProb calculates values in Table C.8.
C
	program StudT
	Common/student/nu,gam
	External StudTProb, GaussProb
	Real t, nu, Tp, Gp
  100 print  *,'Type nDOF and t = |mu-x|/sigma. Terminate with ctrl C '
	read *, n, t
	nu = n
	Tp = StudTProb(t)
	Gp = GaussProb(t)
	print *
	print 1100, 100*Tp, 100*(1-TP)
	print 1200, 100*Gp, 100*(1-GP)
	goto 100
 1100 format(' Prob (Student''s t) = ',f6.1, '%, 1-Prob = ',F5.2,'%')
 1200 format(' Prob (Gaussian)    = ' ,f6.1, '%, 1-Prob = ',F5.2,'%')
	END
	Include C:\f\Append-C\GausProb.for
	Include C:\f\Append-C\StudtPrb.for

