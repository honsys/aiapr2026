C Program D.6: \Append-D\QuikScrp.For
C Write graphics script file for Qdisplay - Screen printer graphics
C -----------------------INITIALIZATION ROUTINES----------------------
	Subroutine Border(x1,y1,x2,y2)
	Real x1,y1,x2,y2
	  Write(5, *) 'Border'
	  Write(5, *) x1, y1, x2, y2
	  Return
	END

	Subroutine InitDisplay(printPic, scriptFile, workDrive, workSpace)
C
C  printPic  = .TRUE. for hardCopy, .FALSE.  for screen
C  workDrive : work space for temporary files
C              (e.g., 'A' or 'B' or 'C', etc. '' for current directory)
C  workSpace : memory allocation for drivers.
C              min = 16k, workSpace =  0 defaults to 32k
	Logical        printPic
	Character*(*)  scriptFile
	Character      workDrive
	Integer        workSpace
	Character*20 scriptFileName
	Common/QukScrip/scriptFileName
	  scriptFileName = scriptFile
	  Open(Unit=5, file=scriptFile)
	  write(5,*) 'InitDisplay'
	  write(5,*) printPic,  workDrive, workSpace
	  Return
	END

	Subroutine InitGraphics(logOn,graphDriverPath)
	Logical  logOn
	Character*(*) graphDriverPath
	  Write(5,*) 'InitGraphics'
	  if (logOn)  then
	    Write(5,*) ' TRUE ',' ',graphDriverPath
	  else
	    Write(5,*) ' FALSE',' ',graphDriverPath
	  endIf
	  Return
	END

	Subroutine SetDefaultColors(c0,c1,c2,c3,c4,c5,c6,c7)
	Integer c0,c1,c2,c3,c4,c5,c6,c7
	  Write(5,*) 'DefaultColors'
	  Write(5,*) c0,c1,c2,c3,c4,c5,c6,c7
	  Return
	END

	Subroutine SetMono(fgPal, bkgPal, fgCol, bkgCol)  !{Print Screen}
	Integer fgPal, bkgPal, fgCol, bkgCol
	  Write(5,*) 'MonoPlot'
	  Write(5,*) fgPal, bkgPal, fgCol, bkgCol
	  Return
	END

	Subroutine OpenWindow
	  Write(5,*) 'OpenWindow'
	  Return
	END

C --------------------------TERMINATION ROUTINES----------------------
	Subroutine CloseGraphics
	Character*20 scriptFileName
	Common/QukScrip/scriptFileName
	Integer i
	  Write(5,*) 'CloseGraphics'
	  Close(5)
	  i = Index(scriptFileName,'.')
	  Print 1000, scriptFileName(1:i-1)
 1000   Format(' To plot graph  -----   Type QDISPLAY ', A)
     1
	  Return
	END

	Subroutine CloseWindow
	  Write(5,*) 'CloseWindow'
	  Return
	END

C --------------------------POINT SCALING ROUTINES--------------------
	Integer function  XScale(x)
	Real x
	  Write(5,*) 'XScale'
	  Write(5,*) x
	  Xscale = 0 ! This is not used, but eliminates a compiler warning
	  Return
	END

	Integer function  YScale(y)
	Real y
	  Write(5,*) 'YScale'
	  Write(5,*) y
	  Yscale = 0 ! This is not used, but eliminates a compiler warning
	  Return
	END

	Subroutine SetScreenRange(ix1, iy1, ix2, iy2)
	Integer ix1, iy1, ix2, iy2
	  Write(5,*) 'ScreenRange'
	  Write(5,*) ix1, iy1, ix2, iy2
	  Return
	END

C ----------------------------GRID ROUTINES---------------------------
	Subroutine Xgrid(fullGrid, nTics, nDec)
	Logical fullGrid
	Integer nTics, nDec
	  Write(5,*) 'Xgrid'
	  Write(5,*) fullGrid, nTics, nDec
	  Return
	END

	Subroutine Ygrid(fullGrid, nTics, nDec)
	Logical fullGrid
	Integer nTics, nDec
	  Write(5,*) 'Ygrid'
	  Write(5,*) fullGrid, nTics, nDec
	  Return
	END

	Subroutine SetXYFormat(xForm, yForm)
	Integer xForm, yForm
	  Write(5,*) 'XYFormat'
	  Write(5,*) xForm, yForm
	  Return
	END

C ------------------------LABELLING ROUTINES-------------------------
	Subroutine XLabel(w)           ! max = 50 characters in VGA
	Character*(*) w
	  Write(5,*) 'XLabel'
	  Write(5,*) w
	  Return
	END

	Subroutine YLabel(w)           ! max = 35 characters in VGA
	Character*(*) w
	  Write(5,*) 'YLabel'
	  Write(5,*) w
	  Return
	END

	Subroutine WriteTextXY(x,y,s) ! out text 's' to real coords x,y
	Character*(*) s
	Real x,y
	  Write(5,*) 'WriteTextXY'
	  Write(5,*) x,y,' ',s
	  Return
	END

C ------------------DATA AND CURVE PLOTTING ROUTINES------------------
	Subroutine RCurve(i, x,y)  ! Connect the dots, i = 0 on 1st call
	Integer i
	Real x,y
	  Write(5,*) 'RCurve'
	  Write(5,*) i,x,y
	  Return
	END

	Subroutine RDataCircle(x,y,dr,dy)  ! Vertical line through solid circle
	Real  x,y,dr,dy
	  Write(5,*) 'DataCircle'
	  Write(5,*) x,y,dr,dy
	  Return
	END

	Subroutine RDataCross(x,y,dx,dy)
	Real x, y, dx, dy
	  Write(5,*) 'DataCross'
	  Write(5,*) x, y, dx, dy
	  Return
	END

	Subroutine RHist(x1, y1, x2, y2)       ! Plot a histogram
	Real x1, y1, x2, y2
	  Write(5,*) 'Histogram'
	  Write(5,*) x1, y1, x2, y2
	  x1=x2         ! calling program need not update x1 and y1
	  y1=y2
	  Return
	END

	Subroutine RLine(x1,y1,x2,y2)
	Real x1, y1, x2, y2
	  Write(5,*) 'RealLine'
	  Write(5,*) x1,y1,x2,y2
	  Return
	END

C --------------------------SPLINE ROUTINES---------------------------
	Subroutine SplineMake(nn, d2ydx2A, d2ydx2B, xIn, yIn)
	Integer nn
	Real d2ydx2A, d2ydx2B, xIn(nn), yIn(nn)
	  Write(5,*) 'SplineMake'
	  Write(5,*) nn, d2Ydx2A, d2ydx2B
	  do 100 i = 1, nn
	    Write(5,*) xIn(i), yIn(i)
  100   continue
	  Return
	END

	Subroutine SCurve(pt1, ptn, nInts, XoffSet, x)
	Integer pt1, ptn, nInts
	Real XoffSet, x(ptn)
	  Write(5,*) 'SplineCurve'
	  Write(5,*) pt1, ptn-pt1+1, nInts, XoffSet
	  do 100 i = pt1, ptn
	    Write(5,*) x(i)
  100   continue
	  Return
	END

C ----------------------------Graphics Controls-----------------------
	Subroutine qSetLineStyle(style, pattern, thickness)
	Integer style, pattern, thickness
	  Write(5,*) 'LineStyle'
	  Write(5,*) style,pattern,thickness
	  Return
	END

	Subroutine qSetTextJustify(Horiz, Vert)
	Integer Horiz, Vert
	  Write(5,*) 'TextJustify'
	  Write(5,*) Horiz, Vert
	  Return
	END

	Subroutine qSetTextStyle(font, dir, size)
	Integer font, dir, size
	  Write(5,*) 'TextStyle'
	  Write(5,*) Font, Dir, Size
	  Return
	END

C ---------------------------Hard Copy Functions----------------------
	Subroutine SelectPrinter(driverC, portC, qSize, qResolution)
	Character*(*) driverC, qSize, portC
	Character qResolution
	  Write(5,*) 'SelectPrinter'
	  Write(5,*) driverC,portC, qSize,qResolution
	  Return
	END

	Subroutine SetSerialPort(baudC,parityC,dataBitsC, stopBitsC, XonC)
	Character*(*) baudC
	Character parityC, dataBitsC, stopBitsC, XonC
	  Write(5,*) 'SetSerialPort'
	  Write(5,*) baudC, parityC, dataBitsC, stopBitsC, XonC
	  Return
	END
	Include \F\Append-D\GraphHst.for
