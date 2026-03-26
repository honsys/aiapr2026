# url2url.g
# Gem URL Redirect WebApp
# -----------------------

# Define our default redirect mapping
_redirects = {
    "/google": "https://www.google.com",
    "/github": "https://github.com/ccbevington/gem",
    "/gemini": "https://deepmind.google/technologies/gemini/",
    "/openai": "https://www.openai.com"
}

# Process CLI arguments
_args = sys.args()
sys.print("Args:", _args)
_idx = 0
if _args[0] == "gem" or _args[0] == "/usr/local/bin/gem"
    _idx = 2
else
    _idx = 1
end

_arg1 = _args[_idx]
_arg2 = _args[_idx + 1]

if _arg1 != nil
    if _arg2 != nil
        _path = _arg1
        _target = _arg2
    else
        _path = "/"
        _target = _arg1
    end
    
    # Ensure target starts with http/https
    # If not, we could add it, but for now let's just warn or assume it's okay for curl
    
    sys.print("Verifying destination: " + _target + "...")
    # Use curl to check if destination is alive and accepting connections
    # -I for HEAD request, -s for silent, -f for fail on HTTP error, -o /dev/null to discard output
    # --connect-timeout 5 to avoid hanging
    _res = sys.exec("curl -I -s -f --connect-timeout 5 -o /dev/null " + _target)
    
    if _res != 0
        sys.print("ERROR: Destination " + _target + " is not alive or not accepting connections (Exit Code: " + _res + ").")
        sys.exit()
    end
    
    sys.print("Destination is alive and accepting connections.")
    _redirects[_path] = _target
end

# Initialize routes with a home page
_routes = {
    "/": "<html><head><title>Gem Redirector</title><style>body{font-family:sans-serif;padding:50px;}a{color:#0066cc;text-decoration:none;}a:hover{text-decoration:underline;}</style></head><body><h1>Gem URL Redirector</h1><p>Short URLs defined:</p><ul>"
}

# Build home page list and redirection routes
for _path, _target in _redirects
    _routes["/"] = _routes["/"] + "<li><a href='" + _path + "'>" + _path + "</a> -> " + _target + "</li>"
    _routes[_path] = "<html><head><meta http-equiv='refresh' content='0; url=" + _target + "'></head><body><p>Redirecting to <a href='" + _target + "'>" + _target + "</a>...</p></body></html>"
end

_routes["/"] = _routes["/"] + "</ul><hr><p>Powered by Gem Language</p></body></html>"

int _port = 8091
sys.print("Starting URL Redirect WebApp on port", _port)
www.app(_port, _routes)

sys.print("WebApp is running at http://localhost:" + _port)
sys.print("Press Ctrl+C to terminate.")

# Keep the main thread alive as www.app runs in a detached thread
while true
    sys.exec("sleep 10")
end
