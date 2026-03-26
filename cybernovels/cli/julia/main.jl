# This file is the main entry point for the Julia native CLI app.
# It assumes it's being run from the root of the julia/ directory.
push!(LOAD_PATH, "./DoconvrtNative/src")
using DoconvrtNative

function main()
    s = ArgParseSettings(description="Native document converter (Julia version).")
    @add_arg_table! s begin
        "input"
            help = "Input file."
            required = true
        "--output", "-o"
            help = "Output file."
            required = true
    end
    
    parsed_args = parse_args(ARGS, s)
    
    try
        DoconvrtNative.convert_file(parsed_args["input"], parsed_args["output"])
        println("Successfully converted '$(parsed_args["input"])' to '$(parsed_args["output"])'")
    catch e
        println(stderr, "Error: ", e)
        exit(1)
    end
end

main()
