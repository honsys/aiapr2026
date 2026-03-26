# Gem Tutorial 24: DevOps, Containers, and VMs
# ---------------------------------------------

# Gem isn't just for data science; it's also a powerful DevOps tool.
# This tutorial demonstrates Docker, Kubernetes (K3s), and Vagrant integration.

# 1. Docker Containers
# --------------------
sys.print("--- Docker Demo ---")
sys.print("Running 'hello-world' container...")
container.docker_run("hello-world", "")

sys.print("Listing running containers...")
container.docker_ps()


# 2. Kubernetes (K3s)
# -------------------
# Apply a simple Nginx deployment manifest
sys.print("\n--- Kubernetes (K3s) Demo ---")
yaml_manifest = "
apiVersion: v1
kind: Pod
metadata:
  name: gem-nginx
  labels:
    app: gem-demo
spec:
  containers:
  - name: nginx
    image: nginx:alpine
    ports:
    - containerPort: 80
"

sys.print("Applying K3s manifest...")
# Note: This requires 'kubectl' to be installed and configured
# res = container.k3s_apply(yaml_manifest)
# if res
#   sys.print("Manifest applied successfully.")
#   container.k3s_get("pods")
# end


# 3. Vagrant VMs
# --------------
# Manage virtual machines directly from Gem scripts.
sys.print("\n--- Vagrant VM Demo ---")

# Initialize a new VM (using a lightweight box)
# vm.init("hashicorp/bionic64")

# Start the VM
# sys.print("Starting VM (this may take a while)...")
# vm.up()

# Check Status
# vm.status()

# Run a command on the VM
# sys.print("Running 'uname -a' on VM...")
# vm.ssh("uname -a")

# Destroy the VM
# sys.print("Destroying VM...")
# vm.destroy()


# 4. New Language Translations
# ----------------------------
# Gem now supports translating Go and Ruby code!

# Example Go Code
go_code = "
package main
import \"fmt\"
func main() {
    fmt.Println(\"Hello from Go!\")
}
"
# To translate: gem translate main.go

# Example Ruby Code
ruby_code = "
def greet(name)
  puts \"Hello, #{name}\"
end
"
# To translate: gem translate script.rb

sys.print("\nDevOps tutorial complete.")
