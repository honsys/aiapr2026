C Program 10.2 \Chapt-10\MaxLIncl.for
C Include file for MaxLike
	Parameter(maxEvents = 2000)            !max number of events
	Real  c
C -Set or used in FetchData -------------
	Integer nTrials
	Real loSearch, hiSearch, tauStep       ! search range and step
	Real xLo, xhi, yLo, yHi                ! plotting range

C -Input data from file -------------See Fig 5.4--
C -for d1 and d2, the short and long fiducial volume cutoffs
C -xProduction and xDecay correspond to vertices V1 and V2.
C ---------------------------------------------------------------------
	Integer nEvents
	Real mass, d1, d2, pLab
	Real xProduction, xDecay, LtoTscale
C -Data arrays for search -------------
	Real times(maxEvents),
     1         LoTlim(maxEvents), HiTlim(maxEvents)	!low and high time limits
C ---------------------------------------------------------------------
	Common/MaxlIncl/
     1 nTrials,nEvents,
     2 loSearch, hiSearch, tauStep, xLo, xHi, yLo, yHi,
     3 mass, d1, d2, pLab,
     4 xProduction, xDecay, times, LtoTscale,
     5 c,                                          ! vel of light*1e-8
     6 hitLim, loTLim