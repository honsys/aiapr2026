# test_redirect.g
# Test for URL Redirect mapping logic

sys.print("--- Testing Gem URL Redirect Logic ---")

_redirects = { "/test": "http://example.com" }
_routes = {}

for _path, _target in _redirects
    _routes[_path] = "<html><head><meta http-equiv='refresh' content='0; url=" + _target + "'></head></html>"
end

# Check if the route was generated correctly
if _routes["/test"] == "<html><head><meta http-equiv='refresh' content='0; url=http://example.com'></head></html>"
    sys.print("PASSED: Redirect route generated correctly.")
else
    sys.print("FAILED: Redirect route generation error.")
    sys.print("Actual:", _routes["/test"])
    sys.exit()
end

sys.print("--- All Tests Passed ---")
