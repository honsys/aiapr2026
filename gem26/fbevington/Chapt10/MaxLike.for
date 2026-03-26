C Program 10.1: \Chapt-10\MaxLike.For
C Direct maximum likelihood example
C  Uses Fitutil, GenUtil, QuikScrp
	Program MaxLike
	Real     sigTau, tauMax, maxM  ! M is log of likelihood function
	Include '\f\Chapt-10\MaxlIncl.For'
	  Call GetData('c:\f\Chapt-10\Test.Dat')
	  pause
	  Call Search(tauMax, maxM)
	  Call WriteOutPut(sigTau, tauMax, maxM)
	END

	Subroutine GetData(inFile)
	Integer   iEvNum
	Character*(*) inFile
	character*80 title
	Include '\f\Chapt-10\MaxlIncl.For'
	c = 3.00
	loSearch = 0.50
	hiSearch = 1.5    !search range
	tauStep  = 0.01
	xLo      = 0.50   ! plot range
	xHi      = 1.2
	yLo      = 0.0
	yHi      = 1.2
	nTrials  = (hiSearch - loSearch)/tauStep
	Open(5, inFile)        ! input data file
	Read(5, 1000) title
 1000 format(A80)
	Print *,'  ',title
	Read(5, *) nEvents, mass, d1, d2
	ievNum = 1
	nEvents = 0
	do while (ievNum .GT.0)
	   Read(5, *) iEvNum, xProduction, pLab, xDecay
	   if (ievNum .GT.0) then
		if ((xDecay .ge. d1) .and. (xdecay .lt. d2)) then
		   nEvents = 1 + nEvents
		   LtoTscale =  mass/(c*pLab)  ! = 1/(c*beta*gamma)
		   times(nEvents) =  (xDecay - xProduction)*LtoTscale ! proper time
C       Convert d1 and d2 to time limits, LoTlim and hiTlim,
C       i.e., integration limits in proper time from the production vertex.
		   loTlim(nEvents) = (d1 - xProduction)*LtoTscale
		   hiTlim(nEvents) = (d2 - xProduction)*LtoTscale
		endIf
	   endIf
	endDo
	Print *, 'End of File - ', nEvents, ' events read'
	Return
	END

	Real Function LogProb(k, tau)
	Integer k
	Real tau
	Real a, b
	Include '\f\Chapt-10\MaxlIncl.For'
C -d1 and d2 are beginning and end of the fiducial region.
C -Must cvt to loTlim and hiTlim which are integration limits in proper time,
C -measured from prod vertex.
C -Now, calc probability-
	b = exp(-hiTlim(k)/tau)
	a = exp(-loTlim(k)/tau)
	Prob = exp(-times(k)/tau)/(tau*(a - b))
	LogProb = aLog(Prob)
	Return
	END

	Real Function LogLike(t)
	Real t, LogProb
	Integer  i
	Real     M, prob
	Include '\f\Chapt-10\MaxlIncl.For'
	  M = 0.0
	  do 100 i = 1, nEvents
	    prob = LogProb(i,t)
	    M = prob + M
  100   continue
	  LogLike = M
	  Return
	END

	Subroutine Search(tauAtMax, maxM)
	Real tauAtMax, maxM
	Integer  trial
	Real M1, M2, M3, del1, del2, delta1, tau, mLikeLi, LogLike
	Include '\f\Chapt-10\MaxlIncl.For'
	  M2    =  -1000
        maxM  =  -1.0e20
	  tau   =   loSearch
	  do 100 trial = 0, nTrials
	    mLikeLi = LogLike(tau)
	    print 1100, trial, tau, mLikeLi
 1100     format(' trial #', I4, ' tau='F6.2, ' log(Like)='F10.3)
	    M3 = mLikeLi
	    if (M3 .GT. M2 )  then   ! Remember, these are negative
		M1 = M2
		M2 = M3
          else                    ! leaving maximum
C  Find maximum of parabola defined by last three points-
            del1     = M2 - M1
            del2     = M3 - 2*M2 + M1
		delta1   = tauStep * (del1/del2 + 1.5)
            tau      = tau  - delta1
            tauAtMax = tau
            maxM     = LogLike(tau)    ! at  maximum of parabola
		Return
          endIf
          tau = tau + tauStep
  100   continue
	  Return
	END

	Real Function Error(t, dt) ! 1/sqrt(-2nd derivative of log(L))
	Real t, dt
	Real  t1, t2, d2Ydt2, err, LogLike
        t1 = t - dt
        t2 = t + dt
        d2Ydt2 = (LogLike(t2) - 2*LogLike(t) + LogLike(t1))/dt**2
	  err    = 1/sqrt(-d2Ydt2)
        Error  = err
	  Return
	END

	Subroutine WriteOutPut(dTau, tauAtMax, maxM)
	Real dTau, tauAtMax, maxM, Error
	Include '\f\Chapt-10\MaxlIncl.For'
	  dTau = Error(tauAtMax, tauStep)
	  Print *, 'n-events=', nEvents,',      max log likelihood=',maxM
	  Print 1000,tauAtMax,dtau
 1000   format(' tau at max =', F6.2, '+-', F4.2)
	  Return
	END

		Include '\f\Chapt-6\FitUtil.For'

