#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <readline/readline.h>
#include <readline/history.h>
#include "gem_interpreter.hpp"

namespace fs = std::filesystem;

std::shared_ptr<GemObject> GemObject::sysInstance = nullptr;

// Simple Tee Stream to write to two streams at once
struct TeeBuffer : public std::streambuf {
    std::streambuf *sb1, *sb2;
    TeeBuffer(std::streambuf *sb1, std::streambuf *sb2) : sb1(sb1), sb2(sb2) {}
    virtual int overflow(int c) {
        if (c == EOF) return !EOF;
        int r1 = sb1->sputc(c);
        int r2 = sb2->sputc(c);
        return (r1 == EOF || r2 == EOF) ? EOF : c;
    }
    virtual int sync() {
        int r1 = sb1->pubsync();
        int r2 = sb2->pubsync();
        return (r1 == 0 && r2 == 0) ? 0 : -1;
    }
};

// Main entry point for the gem language REPL and compiler
int main(int argc, char* argv[]) {
    GemInterpreter interpreter;
    interpreter.setArgs(argc, argv);

    // 1. Check for embedded payload (Self-contained executable mode)
    std::string selfPath = argv[0];
    if (fs::exists(selfPath)) {
        std::ifstream selfFile(selfPath, std::ios::binary);
        if (selfFile.good()) {
            const std::string m1 = "GEM_PAYLOAD_START";
            const std::string m2 = "_MAGIC_STRING_89234";
            std::string marker = m1 + m2;
            
            selfFile.seekg(0, std::ios::end);
            size_t fileSize = selfFile.tellg();
            if (fileSize > 0) {
                size_t checkSize = std::min((size_t)5 * 1024 * 1024, fileSize); // 5MB check
                selfFile.seekg(fileSize - checkSize);
                std::vector<char> buffer(checkSize);
                selfFile.read(buffer.data(), checkSize);
                std::string content(buffer.data(), checkSize);
                
                size_t pos = content.rfind(marker);
                if (pos != std::string::npos) {
                    std::string payload = content.substr(pos + marker.length());
                    if (payload[0] == '\n') payload = payload.substr(1);

                    // Parse multi-file payload
                    size_t fpos = 0;
                    std::string mainScript = "";
                    bool foundFiles = false;

                    while ((fpos = payload.find("GEM_PAYLOAD_FILE: ", fpos)) != std::string::npos) {
                        foundFiles = true;
                        size_t nameStart = fpos + 18;
                        size_t nameEnd = payload.find("\n", nameStart);
                        std::string filename = payload.substr(nameStart, nameEnd - nameStart);
                        
                        size_t contentStart = nameEnd + 1;
                        size_t contentEnd = payload.find("\nGEM_PAYLOAD_FILE_END", contentStart);
                        std::string fileContent = payload.substr(contentStart, contentEnd - contentStart);

                        if (mainScript.empty() && filename.find(".g") != std::string::npos) {
                            mainScript = fileContent;
                        } else if (filename.find(".gm") != std::string::npos) {
                            // Write module to a temporary file in ~/.gem/lib or current dir
                            std::ofstream modFile(filename);
                            modFile << fileContent;
                            modFile.close();
                        }
                        fpos = contentEnd + 21;
                    }

                    if (!mainScript.empty()) {
                        interpreter.currentFile = "embedded_main";
                        std::stringstream ss(mainScript);
                        std::string line;
                        while (std::getline(ss, line) && interpreter.running) {
                            interpreter.execute(line);
                        }
                        return 0;
                    } else if (!foundFiles && !payload.empty()) {
                        // Fallback for single-file legacy payloads
                        interpreter.currentFile = "embedded_legacy";
                        std::stringstream ss(payload);
                        std::string line;
                        while (std::getline(ss, line) && interpreter.running) {
                            interpreter.execute(line);
                        }
                        return 0;
                    }
                }
            }
        }
    }

    // 2. Compile Mode: gem -c <main.g> [modules...] [-o output]
    if (argc > 1 && std::string(argv[1]) == "-c") {
        if (argc < 3) {
            std::cerr << "Usage: gem -c <main.g> [modules...] [-o output_name]" << std::endl;
            return 1;
        }

        std::vector<std::string> sourceFiles;
        std::string outputName = "";
        
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "-o" && i + 1 < argc) {
                outputName = argv[++i];
            } else {
                sourceFiles.push_back(arg);
            }
        }

        if (sourceFiles.empty()) {
            std::cerr << "Error: No source files specified." << std::endl;
            return 1;
        }

        if (outputName.empty()) {
            outputName = fs::path(sourceFiles[0]).stem().string();
        }

        std::cout << "Compiling to " << outputName << "..." << std::endl;

        // Copy current executable to output
        std::ifstream src(argv[0], std::ios::binary);
        if (!src.is_open()) {
             std::cerr << "Error: Could not open gem binary for reading." << std::endl;
             return 1;
        }
        std::ofstream dst(outputName, std::ios::binary);
        dst << src.rdbuf();
        src.close();

        // Append Marker
        const std::string m1 = "GEM_PAYLOAD_START";
        const std::string m2 = "_MAGIC_STRING_89234";
        std::string marker = m1 + m2;
        dst.seekp(0, std::ios::end);
        dst << marker << "\n";

        for (const auto& sourcePath : sourceFiles) {
            if (!fs::exists(sourcePath)) {
                std::cerr << "Warning: Source file " << sourcePath << " not found. Skipping." << std::endl;
                continue;
            }
            std::ifstream scriptFile(sourcePath);
            std::stringstream scriptStream;
            scriptStream << scriptFile.rdbuf();
            
            // Append each file with a header to identify it
            dst << "GEM_PAYLOAD_FILE: " << fs::path(sourcePath).filename().string() << "\n";
            dst << scriptStream.str() << "\nGEM_PAYLOAD_FILE_END\n";
        }
        dst.close();

        // Make executable
        fs::permissions(outputName, fs::perms::owner_all | fs::perms::group_read | fs::perms::group_exec | fs::perms::others_read | fs::perms::others_exec);

        std::cout << "Successfully compiled to " << outputName << std::endl;
        return 0;
    }

    // 4. History Mode: gem -h
    if (argc > 1 && std::string(argv[1]) == "-h") {
        interpreter.handleHistory();
        return 0;
    }

    // 3. Translate Mode: gem translate <file> [-o output] OR gem -t <file> [-o output]
    if (argc > 1 && (std::string(argv[1]) == "translate" || std::string(argv[1]) == "-t")) {
        if (argc < 3) {
            std::cerr << "Usage: gem -t <file.py/r/jl/for/cpp> [-o output.g]" << std::endl;
            return 1;
        }
        std::string sourcePath = argv[2];
        std::string outputName = "";
        if (argc >= 5 && std::string(argv[3]) == "-o") {
            outputName = argv[4];
        }

        if (outputName.empty()) {
            fs::path p(sourcePath);
            // Default to .g for translations unless it looks like a module
            outputName = p.stem().string() + ".g";
        }

        if (!fs::exists(sourcePath)) {
            std::cerr << "Error: Source file " << sourcePath << " not found." << std::endl;
            return 1;
        }

        std::string extension = fs::path(sourcePath).extension().string();
        std::string language = "";
        if (extension == ".py") language = "Python";
        else if (extension == ".r") language = "R";
        else if (extension == ".jl") language = "Julia";
        else if (extension == ".for" || extension == ".f90") language = "Fortran";
        else if (extension == ".cpp" || extension == ".cc" || extension == ".c" || extension == ".h") language = "C++";
        else if (extension == ".go") language = "Go";
        else if (extension == ".rb") language = "Ruby";
        else if (extension == ".rs") language = "Rust";
        else if (extension == ".js") language = "Node.js";
        else {
            std::cerr << "Error: Unsupported extension " << extension << std::endl;
            return 1;
        }

        std::ifstream file(sourcePath);
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();

        if (interpreter.builtins.count("ai")) {
            std::cout << "Translating " << language << " to Gem..." << std::endl;
            std::string prompt = "Translate the following " + language + " code into Gem language. Gem uses 'fun name(params) ... end' for functions, 'sys.print' for output, and supports basic math and objects. \n\n"
                                 "CRITICAL INSTRUCTION for C++: If the C++ code uses multiple inheritance (e.g., class C : public A, public B), Gem only supports single inheritance. You MUST translate this as a chain of single inheritance objects (e.g., obj B_Ext : A ... end, obj C : B_Ext ... end) to preserve the combined functionality. \n\n"
                                 "Only return the Gem code without any explanation:\n\n" + source;
            
            auto aiObj = interpreter.builtins["ai"];
            auto translatedVal = aiObj->call("prompt", {std::make_shared<GemValue>(prompt)});
            
            if (std::holds_alternative<std::string>(translatedVal->value)) {
                std::string translatedCode = std::get<std::string>(translatedVal->value);
                
                // Cleanup Markdown
                if (translatedCode.find("```") != std::string::npos) {
                     size_t first = translatedCode.find("```");
                     size_t last = translatedCode.find("```", first + 3);
                     if (first != std::string::npos && last != std::string::npos) {
                         translatedCode = translatedCode.substr(first + 3);
                         size_t nextLine = translatedCode.find('\n');
                         if (nextLine != std::string::npos && last > nextLine) {
                             translatedCode = translatedCode.substr(nextLine + 1, last - nextLine - 1);
                         } else if (nextLine != std::string::npos) {
                             translatedCode = translatedCode.substr(nextLine + 1);
                         }
                     }
                }

                if (!outputName.empty()) {
                    std::ofstream outFile(outputName);
                    outFile << translatedCode;
                    std::cout << "Translation saved to " << outputName << std::endl;
                } else {
                    std::cout << "--- Translated Code ---" << std::endl;
                    std::cout << translatedCode << std::endl;
                }
                return 0;
            } else {
                std::cerr << "Error: AI translation returned non-string value." << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Error: AI module not loaded." << std::endl;
            return 1;
        }
    }

    // Check for script file argument (simple compiler/runner)
    if (argc > 1) {
        // Run file mode
        std::string filename = argv[1];
        
        // Enforce extension support for .g and .gm
        if (filename.find(".g") == std::string::npos && filename.find(".gm") == std::string::npos) {
            std::cerr << "Error: Gem only supports .g (apps) and .gm (modules) extensions." << std::endl;
            return 1;
        }

        interpreter.currentFile = filename;
        std::ifstream file(filename);
        if (!file.good()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return 1;
        }
        
        std::string line;
        while (std::getline(file, line) && interpreter.running) {
            interpreter.execute(line);
        }
        return 0;
    }

    // REPL mode
    std::cout << "Gem Language REPL (v0.1)" << std::endl;
    std::cout << "Type 'exit' or 'quit' to leave." << std::endl;

    // Setup logging and history in ~/.gem/
    const char* home = std::getenv("HOME");
    fs::path gemDir;
    fs::path libDir;
    fs::path historyFile;
    if (home) {
        gemDir = fs::path(home) / ".gem";
        libDir = gemDir / "lib";
        historyFile = gemDir / "history.txt";
        
        if (!fs::exists(gemDir)) fs::create_directory(gemDir);
        if (!fs::exists(libDir)) fs::create_directory(libDir);

        // Load history
        if (fs::exists(historyFile)) {
            read_history(historyFile.c_str());
        }
    }

    char* input = nullptr;
    while (interpreter.running) {
        input = readline("gem> ");
        if (!input) break; // EOF (Ctrl+D)
        
        std::string line(input);
        free(input); // Free the memory allocated by readline

        if (line.empty()) continue;
        add_history(line.c_str());

        if (!gemDir.empty()) {
            // Save history after each command for persistence
            write_history(historyFile.c_str());

            auto now = std::chrono::system_clock::now();
            auto time_t_now = std::chrono::system_clock::to_time_t(now);
            std::tm* tm_now = std::localtime(&time_t_now);
            char buf[32];
            std::strftime(buf, sizeof(buf), "%Y%j", tm_now); // Julian Date: YYYYDDD
            
            std::string logFilename = std::string(buf) + ".txt";
            std::ofstream logFile(gemDir / logFilename, std::ios::app);
            if (logFile.is_open()) {
                char timeBuf[32];
                std::strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", tm_now);
                logFile << "[" << timeBuf << "] gem> " << line << std::endl;
                
                // Redirect std::cout to both stdout and log file
                std::streambuf* oldCoutBuf = std::cout.rdbuf();
                TeeBuffer teeBuf(oldCoutBuf, logFile.rdbuf());
                std::cout.rdbuf(&teeBuf);
                
                interpreter.execute(line);
                
                std::cout.rdbuf(oldCoutBuf); // Restore std::cout
                logFile.flush();
                continue;
            }
        }

        interpreter.execute(line);
    }

    return 0;
}
