#include "converter.h"
#include <fstream>
#include <stdexcept>
#include <cmark.h>

void convert_md_to_html(const std::string& in_path, const std::string& out_path) {
    std::ifstream ifs(in_path);
    if (!ifs) {
        throw std::runtime_error("Failed to open input file.");
    }
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    char *html = cmark_markdown_to_html(content.c_str(), content.length(), 0);
    
    std::ofstream ofs(out_path);
    if (!ofs) {
        free(html);
        throw std::runtime_error("Failed to open output file.");
    }
    ofs << html;
    free(html);
}

void convert_file(const std::string& input_path, const std::string& output_path) {
    size_t in_dot = input_path.rfind('.');
    size_t out_dot = output_path.rfind('.');
    std::string in_ext = (in_dot == std::string::npos) ? "" : input_path.substr(in_dot);
    std::string out_ext = (out_dot == std::string::npos) ? "" : output_path.substr(out_dot);

    std::cout << "Native conversion from '" << in_ext << "' to '" << out_ext << "'" << std::endl;

    if (in_ext == ".md" && out_ext == ".html") {
        convert_md_to_html(input_path, output_path);
    } else if (out_ext == ".pdf") {
         throw std::runtime_error("Native PDF generation is not supported in the C++ version due to its complexity. Please use the pandoc-based converter.");
    }
    else {
        throw std::runtime_error("This native conversion is not supported.");
    }
}
