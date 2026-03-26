# Gem Comprehensive Test Suite (Verbose - Diagnostic)
# ----------------------------------------------------

_tests_passed = 0
_tests_failed = 0

sys.print("=======================================")
sys.print("   GEM LANGUAGE COMPREHENSIVE TESTS    ")
sys.print("=======================================")

# 1. Keywords & Core Syntax
sys.print("\n[SECTION] Keywords & Core Syntax")

int i = 10
if i == 10
    _tests_passed = _tests_passed + 1
    sys.print("[PASS] int declaration")
end
# Manual check because interpreter skipping is buggy
if i != 10
    _tests_failed = _tests_failed + 1
    sys.print("[FAIL] int declaration")
end

double d = 3.14159
if d == 3.14159
    _tests_passed = _tests_passed + 1
    sys.print("[PASS] double declaration")
end

string s = "Gemstone"
if s == "Gemstone"
    _tests_passed = _tests_passed + 1
    sys.print("[PASS] string declaration")
end

# 2. System (sys)
sys.print("\n[SECTION] Builtin: sys")
if sys.host() != ""
    _tests_passed = _tests_passed + 1
    sys.print("[PASS] sys.host returns data")
end

# 3. Math
sys.print("\n[SECTION] Builtin: math")
if math.sqrt(144) == 12
    _tests_passed = _tests_passed + 1
    sys.print("[PASS] math.sqrt(144)")
end

# 4. Text
sys.print("\n[SECTION] Builtin: text")
if text.sub("hello", 0, 4) == "hell"
    _tests_passed = _tests_passed + 1
    sys.print("[PASS] text.sub(hello, 0, 4)")
end

# 5. Algorithms
sys.print("\n[SECTION] Builtin: algo")
if algo.add(1, 2, 3) == 6
    _tests_passed = _tests_passed + 1
    sys.print("[PASS] algo.add(1, 2, 3)")
end

# 6. File
sys.print("\n[SECTION] Builtin: file")
file.write("vtest.txt", "verbose")
if file.exists("vtest.txt")
    _tests_passed = _tests_passed + 1
    sys.print("[PASS] file.exists after write")
end
sys.exec("rm vtest.txt")

# 7. Finance
sys.print("\n[SECTION] Builtin: fin")
d_ql = fin.date(10, 3, 2026)
# Check exact date format returned by QuantLib
if d_ql.str == "3/10/2026"
    _tests_passed = _tests_passed + 1
    sys.print("[PASS] fin.date.str format")
end
if d_ql.str != "3/10/2026"
    _tests_failed = _tests_failed + 1
    sys.print("[FAIL] fin.date.str format")
    sys.print("  Expected: 3/10/2026")
    sys.print("  Actual:  ", d_ql.str)
end

# 8. AI
sys.print("\n[SECTION] Builtin: ai")
if ai.provider == "gemini"
    _tests_passed = _tests_passed + 1
    sys.print("[PASS] ai.provider default")
end

sys.print("\n=======================================")
sys.print("TEST SUMMARY")
sys.print("---------------------------------------")
sys.print("Total Passed:", _tests_passed)
sys.print("Total Failed:", _tests_failed)
sys.print("=======================================")

if _tests_failed > 0
    sys.print("RESULT: SOME TESTS FAILED")
end
if _tests_failed == 0
    sys.print("RESULT: ALL TESTS PASSED")
end
