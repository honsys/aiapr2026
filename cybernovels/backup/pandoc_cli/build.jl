using PackageCompiler

# The directory of this build script
build_dir = @__DIR__

# The path to the Julia project we want to compile
project_dir = build_dir

# The name of the executable
exe_name = "doconvrt_julia"

# Create the application
create_app(project_dir, joinpath(build_dir, "build_jl"), executables=[exe_name => "doconvrt"], force=true)
