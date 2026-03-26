_r = sys.exec("curl -I -s -f -o /dev/null someinvalidurl")
sys.print("Result:", _r)