# Gem Tutorial 26: DevOps, Containers, and VMs
# This tutorial covers K3s, Docker, Vagrant, Go, and Ruby management.

sys.print("--- DevOps & Container Management ---")

# 1. K3s / Kubernetes
sys.print("Listing K3s nodes:")
container.k3s_nodes()

sys.print("\nListing K3s pods:")
container.k3s_pods()

# Apply a simple nginx deployment (mock/demo)
string nginx_yaml = "
apiVersion: apps/v1
kind: Deployment
metadata:
  name: nginx-deployment
spec:
  selector:
    matchLabels:
      app: nginx
  replicas: 1
  template:
    metadata:
      labels:
        app: nginx
    spec:
      containers:
      - name: nginx
        image: nginx:1.14.2
        ports:
        - containerPort: 80
"
# sys.print("\nApplying Nginx Deployment...")
# container.k3s_apply(nginx_yaml)

# 2. Docker
sys.print("\nListing Docker containers:")
container.docker_ps()

# container.docker_build(".", "my-gem-app")
# container.docker_stop("my-container-id")

# 3. Vagrant VMs
sys.print("\n--- Vagrant VM Management ---")
sys.print("Vagrant status:")
vm.status()

# 4. Golang Interop
sys.print("\n--- Golang Interop ---")
# Using escaped quotes for the import
string go_code = "package main; import \"fmt\"; func main() { fmt.Println(\"Hello from Golang inside Gem!\") }"
sys.print("Running inline Go code:")
go.run(go_code)

# 5. Ruby Interop
sys.print("\n--- Ruby Interop ---")
string ruby_code = "puts \"Hello from Ruby inside Gem!\""
sys.print("Running inline Ruby code:")
ruby.run(ruby_code)

sys.print("\nTutorial 26 Complete.")
