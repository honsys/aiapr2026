module DoconvrtNative

using Markdown
using ArgParse

function convert_md_to_html(in_path::String, out_path::String)
    open(out_path, "w") do f
        write(f, html(Markdown.parse_file(in_path)))
    end
end

function convert_file(input_path::String, output_path::String)
    in_ext = last(splitext(input_path))
    out_ext = last(splitext(output_path))

    println("Native conversion from '$(in_ext)' to '$(out_ext)'")

    if in_ext == ".md" && out_ext == ".html"
        convert_md_to_html(input_path, output_path)
    elseif out_ext == ".pdf"
        error("Native PDF generation is not supported in the Julia version due to its complexity. Please use the pandoc-based converter.")
    else
        error("This native conversion is not supported.")
    end
end

end # module
