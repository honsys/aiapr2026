C Program D.7: \Append-D\QhistInc.For
C----- Common for QuikHist --------
	Common /QHists/ maxNHitst, hNumMax, initCalled,
     1 terminal, borderDefined, printCode, histBot, histInt, histTop,
     2 histNbins, histStart, histLabel, histBelow, histIn, histAbove,
     3 histSum, histSum2, histMean, histStDev, histExists, histData,
     4 outDev, fullGrid, xLo, xHi, nXtic, nXdec
	Parameter (maxNHists=20)
	Parameter (maxHistBins=1000)
	integer  hNumMax
	Logical initCalled, terminal, borderDefined
	Real histBot(1:maxNHists), histInt(1:maxNHists),
     1 histTop(1:maxNHists)
	Character printCode
C             # bins,                 pointer to 1st bin of hist
	Integer histNbins(1:maxNHists), histStart(1:maxNHists)
	Integer histBelow(1:maxNHists), histIn(1:maxNHists),
     1 histAbove(1:maxNHists)                         ! where the events went
	Real histSum(1:maxNHists), histSum2(1:maxNHists),
     1 histMean(1:maxNHists), histStDev(1:maxNHists)  ! statistics
	Character*32 histLabel(1:20)                    ! Histogram label
	Logical histExists(1:maxNHIsts)
	Integer histData(0:maxHistBins)                 ! the bins
	Logical fullGrid
	Real xLo, xHi
	Integer nXtic, nxDec
C-----------------------------------------------------------
