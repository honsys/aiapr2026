# Gem CDN Server Daemon
# Usage: gem cdn_server.g [port] [config_file]

# Default configuration
string port = "8080"
string dashboard_port = "8081"
string config_file = ""

# Parse command line arguments
args = sys.args()
if args.length > 0 {
    port = args[0]
}
if args.length > 1 {
    config_file = args[1]
}

sys.print("----------------------------------------")
sys.print("Gem CDN Server v1.0")
sys.print("----------------------------------------")
sys.print("Proxy Port:     " + port)
sys.print("Dashboard Port: " + dashboard_port)
sys.print("Config File:    " + (config_file != "" ? config_file : "None (Default)"))

# Load configuration if provided
if config_file != "" {
    if file.exists(config_file) {
        sys.print("Loading config from " + config_file + "...")
        string content = text.read(config_file)
        
        # Auto-detect config type
        string type = "nginx"
        if content.sub(0, 10).replace("ProxyPass", "MATCH") != content.sub(0, 10) {
            type = "apache"
        }
        
        if cdn.config(type, content) {
            sys.print("Configuration loaded successfully (" + type + ").")
        } else {
            sys.print("Error loading configuration.")
        }
    } else {
        sys.print("Error: Config file not found.")
    }
} else {
    # Default route for testing if no config
    sys.print("No config provided. Setting default route: / -> http://example.com")
    # We can manually set routes if we exposed a set_route method, 
    # or just synthesize a config string.
    cdn.config("nginx", "location / { proxy_pass http://example.com; }")
}

# Start the CDN Proxy Server
sys.print("Starting Proxy Server...")
cdn.start(port)

# Start the Monitoring Dashboard
sys.print("Starting Monitoring Dashboard...")
cdn.dashboard(dashboard_port)

sys.print("----------------------------------------")
sys.print("Server is running.")
sys.print("Press Ctrl+C to stop.")

# Keep the main thread alive
while true {
    # Keep the main thread alive
    sys.exec("sleep 5")
}
