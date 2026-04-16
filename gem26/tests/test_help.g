# test_help.g — Tests for helpfull and helpless REPL features
# Tests: bare keywords, method call forms, return value, file written

_pass = 0
_fail = 0

# --- 1. sys.helpfull() method form: returns filename string ---
string hf1 = sys.helpfull()
if hf1 != ""
    _pass = _pass + 1
    sys.print("[PASS] sys.helpfull() returns filename")
end
if hf1 == ""
    _fail = _fail + 1
    sys.print("[FAIL] sys.helpfull() returned empty string")
end

# --- 2. sys.helpfull() writes a real file ---
if file.exists(hf1)
    _pass = _pass + 1
    sys.print("[PASS] helpfull file exists:", hf1)
end
if !file.exists(hf1)
    _fail = _fail + 1
    sys.print("[FAIL] helpfull file missing:", hf1)
end

# --- 3. helpfull file contains expected content ---
string hf1_content = text.read(hf1)
if rex.match(hf1_content, "Gem Language")
    _pass = _pass + 1
    sys.print("[PASS] helpfull file contains 'Gem Language'")
end
if !rex.match(hf1_content, "Gem Language")
    _fail = _fail + 1
    sys.print("[FAIL] helpfull file missing 'Gem Language'")
end
if rex.match(hf1_content, "## Keywords")
    _pass = _pass + 1
    sys.print("[PASS] helpfull file contains '## Keywords'")
end
if rex.match(hf1_content, "## Builtin Modules")
    _pass = _pass + 1
    sys.print("[PASS] helpfull file contains '## Builtin Modules'")
end

# --- 4. sys.helpless() method form: returns filename string ---
string hf2 = sys.helpless()
if hf2 != ""
    _pass = _pass + 1
    sys.print("[PASS] sys.helpless() returns filename")
end
if hf2 == ""
    _fail = _fail + 1
    sys.print("[FAIL] sys.helpless() returned empty string")
end

# --- 5. sys.helpless() writes a real file ---
if file.exists(hf2)
    _pass = _pass + 1
    sys.print("[PASS] helpless file exists:", hf2)
end
if !file.exists(hf2)
    _fail = _fail + 1
    sys.print("[FAIL] helpless file missing:", hf2)
end

# --- 6. helpfull and helpless produce same content format ---
string hf2_content = text.read(hf2)
if rex.match(hf2_content, "Gem Language")
    _pass = _pass + 1
    sys.print("[PASS] helpless file contains 'Gem Language'")
end

# --- Summary ---
sys.print("---")
sys.print("help tests passed:", _pass)
sys.print("help tests failed:", _fail)
if _fail == 0
    sys.print("RESULT: ALL HELP TESTS PASSED")
end
if _fail > 0
    sys.print("RESULT: SOME HELP TESTS FAILED")
end
