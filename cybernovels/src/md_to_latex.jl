#!/usr/bin/env julia

"""
Converts a Markdown file to a LaTeX file using pandoc.
"""
function main()
    # 1. Check if pandoc is installed
    if isnothing(Sys.which("pandoc"))
        println(stderr, "Error: pandoc is not installed or not in the system's PATH.")
        exit(1)
    end

    # 2. Validate command-line arguments
    if length(ARGS) != 1
        println(stderr, "Usage: $(@__FILE__) <input.md>")
        exit(1)
    end

    input_file = ARGS[1]

    # 3. Check if the input file exists
    if !isfile(input_file)
        println(stderr, "Error: File not found: $(input_file)")
        exit(1)
    end
    
    if !endswith(input_file, ".md")
        println(stderr, "Error: Input file must be a Markdown file (.md): $(input_file)")
        exit(1)
    end

    output_file = replace(input_file, r"\.md$" => ".tex")

    # 4. Construct and run the pandoc command
    cmd = `pandoc $(input_file) -s -o $(output_file) --variable fontsize=14pt --variable documentclass=extarticle --variable geometry:margin=1in`

    try
        run(cmd)
        println("Successfully converted '$(input_file)' to '$(output_file)'")
    catch e
        println(stderr, "Error during pandoc conversion:")
        println(stderr, e)
        exit(1)
    end
end

main()
