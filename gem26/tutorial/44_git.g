# 44_git.g — git builtin tutorial
# Demonstrates: clone, pull, commit, push, gitsync
# Run: ./gem tutorial/44_git.g

sys.print("=== git builtin tutorial ===")

# --- 1. Clone a repository (shallow, with submodules) ---
sys.print("\n--- 1. Clone ---")
# git.clone(url)          — clone into a directory named from the repo
# git.clone(url, dir)     — clone into a specific directory
# Returns exit code (0 = success)
# int rc = git.clone("https://github.com/octocat/Hello-World", "/tmp/hello_world_demo")
# sys.print("clone exit code:", rc)
sys.print("# git.clone(url, [dir])  — shallow clone with submodules")
sys.print("# int rc = git.clone(\"https://github.com/user/repo\", \"/tmp/myrepo\")")

# --- 2. Pull latest changes ---
sys.print("\n--- 2. Pull ---")
# Must be run from inside a git repo directory.
# int rc = git.pull()
# sys.print("pull exit code:", rc)
sys.print("# git.pull()  — git pull (run from inside a repo)")

# --- 3. Stage all and commit ---
sys.print("\n--- 3. Commit ---")
# git.commit()            — commit with default message "sync"
# git.commit("my message")— commit with custom message
# int rc = git.commit("feat: add git tutorial")
# sys.print("commit exit code:", rc)
sys.print("# git.commit([msg])  — git add -A && git commit -m msg")
sys.print("# Default message: \"sync\"")

# --- 4. Push to remote ---
sys.print("\n--- 4. Push ---")
# int rc = git.push()
# sys.print("push exit code:", rc)
sys.print("# git.push()  — git push")

# --- 5. gitsync — one-shot pull + commit + push + show ---
sys.print("\n--- 5. gitsync ---")
# git.gitsync() is the most common workflow:
#   1. git pull
#   2. git add -A && git commit -m "sync"
#   3. git push
#   4. git show --summary --stat
#
# You can also call it as a bare identifier (REPL alias):
#   gitsync
#
# int rc = git.gitsync()
# sys.print("gitsync exit code:", rc)
sys.print("# git.gitsync()  — pull + commit + push + show summary")
sys.print("# Alias: 'gitsync' in REPL calls git.gitsync() directly")

# --- 6. Typical workflow example ---
sys.print("\n--- 6. Typical workflow ---")
sys.print("# Clone a repo, make changes, then sync:")
sys.print("#   git.clone(\"https://github.com/user/project\", \"project\")")
sys.print("#   # ... edit files ...")
sys.print("#   git.gitsync()")
sys.print("#")
sys.print("# Or step by step:")
sys.print("#   git.pull()")
sys.print("#   git.commit(\"fix: update docs\")")
sys.print("#   git.push()")

# --- 7. Language comparisons ---
sys.print("\n--- 7. Language comparisons ---")
sys.print("  Gem:    git.clone(url)  git.pull()  git.commit(msg)  git.push()  gitsync")
sys.print("  Python: import subprocess; subprocess.run(['git', 'clone', url])")
sys.print("          import git; repo = git.Repo('.')  # gitpython")
sys.print("  Julia:  LibGit2.clone(url, path)  # stdlib")
sys.print("  Go:     go-git library or os/exec")
sys.print("  Ruby:   require 'git'; g = Git.open('.')  # git gem")
sys.print("  Rust:   git2::Repository::clone(url, path)  # git2 crate")

sys.print("\n=== git tutorial complete ===")
sys.print("See: help \"git\"  for full reference")
