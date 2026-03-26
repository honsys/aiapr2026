#!/usr/bin/env julia

using ArgParse

function main()
    s = ArgParseSettings(
        description="A universal document converter using pandoc.",
        epilog="""
        Supported formats include:
        - markdown (.md)
        - latex (.tex)
        - pdf (.pdf)
        - html (.html)
        - plain text (.txt)
        """
    )
    @add_arg_table! s begin
        "input"
            help = "Input file to convert"
            required = true
        "--output", "-o"
            help = "Output file"
            required = true
    end
    
    parsed_args = parse_args(ARGS, s)
    
    input_file = parsed_args["input"]
    output_file = parsed_args["output"]

    if isnothing(Sys.which("pandoc"))
        println(stderr, "Error: pandoc is not installed or not in the system's PATH.")
        exit(1)
    end
    
    cmd = `pandoc $(input_file) -o $(output_file) --pdf-engine=pdflatex -V fontsize=12pt -V documentclass=article -V geometry:margin=1in`

    try
        run(cmd)
        println("Successfully converted '$(input_file)' to '$(output_file)'")
    catch e
        println(stderr, "Error converting file: ", e)
        exit(1)
    end
end

# Only run main if this script is executed directly
if abspath(PROGRAM_FILE) == @__FILE__
    main()
end
