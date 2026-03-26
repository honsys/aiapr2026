# Gem Tutorial 27: Rust and Node.js Interop
# This tutorial covers using Rust and Node.js from within Gem.

sys.print("--- Rust & Node.js Interop ---")

# 1. Rust Interop
sys.print("\n--- Rust Interop ---")
string rust_code = "
    println!(\"Hello from Rust inside Gem!\");
"
sys.print("Running inline Rust code:")
rust.run(rust_code)

# 2. Node.js Interop
sys.print("\n--- Node.js Interop ---")
string node_code = "console.log('Hello from Node.js inside Gem!');"
sys.print("Running inline Node.js code:")
node.run(node_code)

# Example: Installing a package (commented out to avoid side effects)
# node.npm_install("express")

sys.print("\nTutorial 27 Complete.")
