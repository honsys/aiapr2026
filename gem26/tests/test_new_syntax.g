a = b = c = 1.0 ; b += 1 ; c += a
sys.print("a: " + a)
sys.print("b: " + b)
sys.print("c: " + c)

# Test chained assignment with expression
x = y = 5 + 5
sys.print("x: " + x)
sys.print("y: " + y)

# Test compound assignments
x += 10 ; y *= 2
sys.print("x after += 10: " + x)
sys.print("y after *= 2: " + y)

# Test object property compound assignment
o = obj()
o.val = 100
o.val += 50
sys.print("o.val: " + o.val)

# Test multiple statements in function
fun test()
  p = 10 ; q = 20
  sys.print("p+q: " + (p+q))
end
test()
