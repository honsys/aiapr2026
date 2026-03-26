obj Parent()
    int p_val = 10
    fun show_p()
        sys.print("Parent p_val: " + .p_val)
    end
end

obj Child() : Parent
    int c_val = 20
    fun test()
        sys.print("Own c_val via . : " + .c_val)
        sys.print("Parent p_val via .. : " + ..p_val)
        ..p_val = 15
        sys.print("Updated Parent p_val via .. : " + ..p_val)
        ..show_p()
    end
end

c = Child()
c.test()
