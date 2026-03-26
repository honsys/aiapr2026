_i = 0
fun loop_cond()
    res = _i < 5
end res

sys.print("Cond for 0:", loop_cond())
_i = 10
sys.print("Cond for 10:", loop_cond())
