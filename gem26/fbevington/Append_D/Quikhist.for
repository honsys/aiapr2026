C Program D.3: \Append-D\QuikHist.For
C Makes histograms. Character or graphics display.
C Uses QuikScrp
      Block Data qHistData
      Include '\f\Append-D\qHistInc.for'
      Data  hNumMax/0/
      Data initCalled/.false./, terminal/.true./,
     1  borderDefined/.false./, printCode/'n'/
      END

C----------  Routines called externally -------------------
      Subroutine HistInit(outPut)
      Character*(*) outPut
      Include '\f\Append-D\qHistInc.for'
      Integer i
      if (outPut .EQ. ' ' )  then      !{terminal display}
      Open(unit=5, file='con')
      else
        Open(unit=5, file=outPut, carriage control='FORTRAN') ! file
        terminal = .FALSE.
      endIf
      do 100 i = 1, 20
        histExists(i) = .FALSE.
  100 continue
      initCalled = .TRUE.
       histStart(1)=0
       Return
       END

      Subroutine HistSetup(hNum, bot, int, top, hLabel)
      Include '\f\Append-D\qHistInc.for'
      Integer hNum, i
      Real bot, int, top
      Character*(*) hLabel
      if (.not. initCalled )
     1  Call QHErrorAbort('HistInit has not been called.')
      if (hNum .GT. maxNhists )
     1  Call QHErrorAbort('Too many histograms requested.')
      histExists(hNum) = .TRUE.
      if (hNum .GT. 1 )
     1  histStart(hNum)=histStart(hNum-1)+histNBins(hNum-1)
      if (hNum .GT. hNumMax ) hNumMax = hNum
      histBot(hNum)=bot
      histInt(hNum)=int
      histTop(hNum)=top
      histNBins(hNum)=Nint((top-bot)/int)
      if ((histStart(hNum)+histNBins(hNum)) .GT. maxHistBins )
     1  Call QHErrorAbort('Too many histogram bins requested.')
      histLabel(hNum) = hLabel
      print 1000, hnum, hLabel
 1000 Format(' Hist #', I3, A60)
      print *, ' Bot = ',bot,', top=',Top,', int=',Int
      do 100 i = histStart(hNum), histStart(hNum)+histNBins(hNum)
      histData(i)=0
  100 continue
      histBelow(hNum) = 0
      histIn(hNum)    = 0
      histAbove(hNum) = 0
      histSum(hNum)   = 0
      histSum2(hNum)  = 0
      Return
      END

      Subroutine Histogram(hNum, x)
      Include '\f\Append-D\qHistInc.for'
      Integer hNum, k
      Real    x, y
      y=(x-histBot(hNum))/histInt(hNum)
      k=Aint(y)
      if (y .LT.  0)  then
        histBelow(hNum) = 1 + histBelow(hnum)
      elseif (k .GE. histNbins(hNum) )  then
        histAbove(hNum) = 1 + histAbove(hNum)
      else
C  ---x is within limits---
        histIn(hNum)           = 1   + histIn(hNum)
        histSum(hNum)          = x   + histSum(hNum)
        histSum2(hNum)         = x*x + histSum2(hNum)
        histData(k+histStart(hNum)) = 1 + histData(k+histStart(hNum))
      endIf
      Return
      END

      Subroutine HistDisplayAll(graph) !Displays all histograms in either mode
      Include '\f\Append-D\qHistInc.for'
      Logical graph
	if (hNumMax .EQ. 0) Call QHerrorAbort('No Histograms saved.')
      Integer  i
      do 100 i = 1, hNumMax
        Call HistDisplay(i)
  100 continue
      Close(outDev)
      Return
      END

C----------  Routines called internally -------------------
      Subroutine HistDisplay(hNum)       !Text-based display
      Include '\f\Append-D\qHistInc.for'
      Integer hNum, i, j, iscl, nMax, remainder
      Character printChar1, printChar2
      if (.NOT. histExists(hNum) ) then
        Print *, 'No Histogram'
      else
        Call HistStats(hNum)
        pause
        nMax=0
        do 100 i = 0 ,HistNbins(hNum)-1
          if (histData(i+HistStart(hNum)) .GT. nMax )
     1      nMax=histData(i+HistStart(hNum))
  100   continue
        iscl=nMax / 50 +1
        if (printCode .EQ. 'n') then
          printChar1=char(iscl+48) !  {# counts}
        else
          printChar1 = printCode
        endIf
        do 200 i=0, histNbins(hNum)-1
 1000 Format(1xF10.4,I8,' ')
          Write(5, 1000) histBot(hNum)+i*histInt(hNum),
     1    histData(i+histStart(hNum))
          if (histData(i+histStart(hNum)).GT.0 ) then
	do 210 j=1, histData(i+histStart(hNum))/iscl
 1100 Format('&'A1)
  210       write(5, 1100) printChar1
          endIf
          remainder = Mod(histData(i+HistStart(hNum)), iscl)
          if (remainder .NE. 0 ) then
	PrintChar2= Char(remainder+48)
	Write(5, 1100) printChar2
          endIf
  200   continue
      endIf
      if (.not.Terminal) close(unit=5)
      Return
      END

      Integer Function HistFetch(hNum, hBin) ! Fetch contents of one bin
      Include '\f\Append-D\qHistInc.for'
      Integer hNum, hBin
      HistFetch=histData(hBin-1+histStart(hNum))
      Return
      END

      Subroutine HistSetBorder(grid, xLow, xHigh, nxticks, nxdecPt)
      Include '\f\Append-D\qHistInc.for'
      Logical grid
      Real xLow, xHigh
      Integer nxTicks, nxdecPt
      fullGrid = grid
      xLo      = xLow
      xHi      = xHigh
      nxTic    = nxTicks
      nxDec    = nxDecPt
      borderDefined = .TRUE.
      Return
      END

      Subroutine HistStats(hNum)
      Include '\f\Append-D\qHistInc.for'
      Integer hNum, num
      Real err
      num=histIn(hNum)
      if (num.NE.0 ) then
        histMean(hNum)= histSum(hNum)/num
      else
        histMean(hNum)= 0
      endIf
      if (num.NE.0 ) then
        histStDev(hNum)=sqrt(histSum2(hNum)/num-(histMean(hNum))**2)
      else
        histStDev(hNum)=0
      endIf
      if (num.NE.0 ) then
        err=HistStDev(hNum)/sqrt(num)
      else
        err=  0
      endIf
      Write(5,*) ' Hist #',hNum
      Write(5,*) histLabel(hNum)
      Write(5,*) ' (Below = ',histBelow(hNum),' ,In = ',histIn(hNum),
     1    ' ,Above=',histAbove(hNum),')'
        Write(5,*) ' Mean = ',HistMean(hNum),
     1    ' StDev = ',HistStDev(hNum),' Err= ',err
      Return
      END

      Subroutine QHerrorAbort(s)
      character*(*) s
      print *,s
      Call Exit
      END

