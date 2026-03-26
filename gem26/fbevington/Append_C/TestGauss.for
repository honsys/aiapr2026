C Test Standard Gaussian Functions  (Table C.1 and C.2)
	Program TestGauss
	real z
  10  Print *, 'Type: z, where z = |x-mu|/sig.(Terminate in ctrl C)  '
	Read *, z
c Calculate probability density
	y = GaussDens(z)
 1000 Format(' Gauss Density= ',F7.4,' for z =',F5.2)
	Print 1000, y, z
c Calculate gaussian probability
	y = GaussProb(z)
	print 1100, y, z
 1100 Format(' Gauss Probability= ',F7.4,' for z =',F4.1)
	goto 10
	END
	Include C:\f\Append-C\GausProb.for
	Include C:\f\Append-A\Simpson.for
