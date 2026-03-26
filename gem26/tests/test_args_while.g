_args = sys.args()
_i = 0
while true
    _v = _args[_i]
    if _v == nil
        # how to exit while?
        # let's try setting _i to a high value if break doesn't exist
        # or maybe break works
        break
    end
    sys.print("Arg", _i, ":", _v)
    _i = _i + 1
end
sys.print("Found", _i, "args")