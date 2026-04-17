#ifndef GEM_ART_HPP
#define GEM_ART_HPP

#include "gem_base.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <array>
#include <stdexcept>

// Run a shell command and capture stdout
static std::string artExec(const std::string& cmd) {
    std::array<char, 256> buf;
    std::string result;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "[error: popen failed]";
    while (fgets(buf.data(), buf.size(), pipe)) result += buf.data();
    pclose(pipe);
    return result;
}

// Read entire file into string
static std::string artReadFile(const std::string& path) {
    std::ifstream f(path);
    if (!f) return "";
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

// Write string to file
static bool artWriteFile(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    if (!f) return false;
    f << content;
    return true;
}

// Convert text to ASCII art using figlet (falls back to plain text)
static std::string textToAsciiArt(const std::string& text, const std::string& font = "standard") {
    // Try figlet first, then toilet, then plain fallback
    std::string cmd = "figlet -f " + font + " " + std::string("\"") + text + "\"" + " 2>/dev/null";
    std::string result = artExec(cmd);
    if (!result.empty()) return result;
    cmd = "toilet -f " + font + " \"" + text + "\" 2>/dev/null";
    result = artExec(cmd);
    if (!result.empty()) return result;
    // Plain fallback: box the text
    std::string border(text.size() + 4, '*');
    return border + "\n* " + text + " *\n" + border + "\n";
}

// Convert Mermaid/markdown file to ASCII art via mmdc → SVG → rsvg-convert → jp2a
// or simpler: render mindmap markdown as indented ASCII tree
static std::string mermaidToAscii(const std::string& mdPath) {
    // Extract mermaid block and render as indented tree
    std::string content = artReadFile(mdPath);
    if (content.empty()) return "[error: cannot read " + mdPath + "]";

    // Try mmdc → SVG → ascii pipeline
    std::string svgPath = "/tmp/gem_art_mindmap.svg";
    std::string cmd = "mmdc -i \"" + mdPath + "\" -o \"" + svgPath + "\" 2>/dev/null";
    artExec(cmd);

    // Try rsvg-convert + jp2a for bitmap→ascii
    std::string pngPath = "/tmp/gem_art_mindmap.png";
    cmd = "rsvg-convert -w 120 \"" + svgPath + "\" -o \"" + pngPath + "\" 2>/dev/null";
    artExec(cmd);
    std::string ascii = artExec("jp2a --width=100 \"" + pngPath + "\" 2>/dev/null");
    if (!ascii.empty()) return ascii;

    // Fallback: parse mermaid mindmap block and render as indented tree
    std::istringstream ss(content);
    std::string line, result;
    bool inMermaid = false;
    while (std::getline(ss, line)) {
        if (line.find("```mermaid") != std::string::npos) { inMermaid = true; continue; }
        if (inMermaid && line.find("```") != std::string::npos) { inMermaid = false; continue; }
        if (inMermaid && line.find("mindmap") == std::string::npos) {
            // Count leading spaces for indentation level
            size_t indent = 0;
            while (indent < line.size() && line[indent] == ' ') indent++;
            std::string trimmed = line.substr(indent);
            if (trimmed.empty()) continue;
            // Strip mermaid node decorators like ((text)) or [text]
            for (auto& ch : std::string("()[]{}")) {
                trimmed.erase(std::remove(trimmed.begin(), trimmed.end(), ch), trimmed.end());
            }
            result += std::string(indent, ' ') + (indent == 0 ? "* " : "- ") + trimmed + "\n";
        }
    }
    return result.empty() ? content : result;
}

// Convert ASCII art text to SVG
static std::string asciiToSvg(const std::string& ascii) {
    std::istringstream ss(ascii);
    std::string line;
    std::vector<std::string> lines;
    size_t maxLen = 0;
    while (std::getline(ss, line)) {
        maxLen = std::max(maxLen, line.size());
        lines.push_back(line);
    }
    int charW = 8, charH = 16;
    int svgW = (int)maxLen * charW + 20;
    int svgH = (int)lines.size() * charH + 20;

    std::ostringstream svg;
    svg << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << svgW << "\" height=\"" << svgH << "\">\n"
        << "  <rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n"
        << "  <text font-family=\"monospace\" font-size=\"14\" fill=\"black\">\n";
    for (size_t i = 0; i < lines.size(); i++) {
        // Escape XML special chars
        std::string escaped;
        for (char c : lines[i]) {
            if      (c == '&')  escaped += "&amp;";
            else if (c == '<')  escaped += "&lt;";
            else if (c == '>')  escaped += "&gt;";
            else if (c == '"')  escaped += "&quot;";
            else                escaped += c;
        }
        svg << "    <tspan x=\"10\" dy=\"" << charH << "\">" << escaped << "</tspan>\n";
    }
    svg << "  </text>\n</svg>\n";
    return svg.str();
}

// Convert SVG file to ASCII art via rsvg-convert + jp2a
static std::string svgToAscii(const std::string& svgPath, int width = 100) {
    std::string pngPath = "/tmp/gem_art_svg2ascii.png";
    std::string cmd = "rsvg-convert -w " + std::to_string(width * 8) +
                      " \"" + svgPath + "\" -o \"" + pngPath + "\" 2>/dev/null";
    artExec(cmd);
    std::string result = artExec("jp2a --width=" + std::to_string(width) + " \"" + pngPath + "\" 2>/dev/null");
    if (!result.empty()) return result;
    // Fallback: try ImageMagick + jp2a
    cmd = "convert -density 150 \"" + svgPath + "\" \"" + pngPath + "\" 2>/dev/null";
    artExec(cmd);
    result = artExec("jp2a --width=" + std::to_string(width) + " \"" + pngPath + "\" 2>/dev/null");
    return result.empty() ? "[error: rsvg-convert/jp2a not available]" : result;
}

// Render README.md or tutorial/README.md as ASCII art summary
static std::string readmeToAscii(const std::string& path) {
    std::string content = artReadFile(path);
    if (content.empty()) return "[error: cannot read " + path + "]";
    // Extract headings and first line of each section
    std::istringstream ss(content);
    std::string line, result;
    result += textToAsciiArt("Gem Language", "small") + "\n";
    while (std::getline(ss, line)) {
        if (line.substr(0, 2) == "##") {
            // Strip leading #s
            size_t i = 0;
            while (i < line.size() && (line[i] == '#' || line[i] == ' ')) i++;
            result += "  >> " + line.substr(i) + "\n";
        } else if (line.substr(0, 1) == "#" && line.size() > 1 && line[1] != '#') {
            size_t i = 0;
            while (i < line.size() && (line[i] == '#' || line[i] == ' ')) i++;
            result += "=== " + line.substr(i) + " ===\n";
        }
    }
    return result;
}

class GemArt : public GemSys {
public:
    GemArt() : GemSys() {
        name = "art";

        // art.text_to_art(text, [font]) → ASCII art string
        methods["text_to_art"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(std::string("[error: text required]"));
            std::string text = args[0]->toString();
            std::string font = (args.size() > 1) ? args[1]->toString() : "standard";
            return std::make_shared<GemValue>(textToAsciiArt(text, font));
        }, true };

        // art.art_to_file(ascii_text, path) → writes text file, returns path
        methods["art_to_file"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 2) return std::make_shared<GemValue>(std::string("[error: art_to_file(text, path)]"));
            std::string content = args[0]->toString();
            std::string path    = args[1]->toString();
            artWriteFile(path, content);
            return std::make_shared<GemValue>(path);
        }, true };

        // art.art_to_svg(ascii_text, [path]) → SVG string (and optionally writes file)
        methods["art_to_svg"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(std::string("[error: ascii text required]"));
            std::string ascii = args[0]->toString();
            std::string svg   = asciiToSvg(ascii);
            if (args.size() > 1) artWriteFile(args[1]->toString(), svg);
            return std::make_shared<GemValue>(svg);
        }, true };

        // art.svg_to_art(svg_path, [width]) → ASCII art string
        methods["svg_to_art"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(std::string("[error: svg_path required]"));
            std::string path = args[0]->toString();
            int width = (args.size() > 1) ? std::stoi(args[1]->toString()) : 100;
            return std::make_shared<GemValue>(svgToAscii(path, width));
        }, true };

        // art.mindmap([path]) → ASCII art of docs/gem_mindmap.md
        methods["mindmap"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            std::string path = (args.size() > 0) ? args[0]->toString() : "docs/gem_mindmap.md";
            return std::make_shared<GemValue>(mermaidToAscii(path));
        }, true };

        // art.inheritance([path]) → open interactive zoom/pan inheritance diagram HTML
        methods["inheritance"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            std::string htmlPath = (args.size() > 0) ? args[0]->toString() : "docs/gem_inheritance.html";
#ifdef __APPLE__
            std::string cmd = "open \"" + htmlPath + "\"";
#elif defined(_WIN32)
            std::string cmd = "start \"\" \"" + htmlPath + "\"";
#else
            std::string cmd = "xdg-open \"" + htmlPath + "\"";
#endif
            std::system(cmd.c_str());
            return std::make_shared<GemValue>(htmlPath);
        }, true };

        // art.readme([path]) → ASCII art summary of README.md
        methods["readme"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            std::string path = (args.size() > 0) ? args[0]->toString() : "README.md";
            return std::make_shared<GemValue>(readmeToAscii(path));
        }, true };

        // art.tutorial([path]) → ASCII art summary of tutorial/README.md
        methods["tutorial"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            std::string path = (args.size() > 0) ? args[0]->toString() : "tutorial/README.md";
            return std::make_shared<GemValue>(readmeToAscii(path));
        }, true };
    }
};

#endif // GEM_ART_HPP
