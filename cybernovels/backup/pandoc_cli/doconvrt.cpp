#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

// Basic check if a command exists by checking PATH.
bool command_exists(const char* cmd) {
    std::string check_cmd = "command -v ";
    check_cmd += cmd;
    check_cmd += " > /dev/null 2>&1";
    return system(check_cmd.c_str()) == 0;
}

int main(int argc, char* argv[]) {
    if (argc != 4 || std::string(argv[2]) != "-o") {
        std::cerr << "Usage: " << argv[0] << " <input-file> -o <output-file>" << std::endl;
        return 1;
    }

    std::string input = argv[1];
    std::string output = argv[3];

    if (!command_exists("pandoc")) {
        std::cerr << "Error: pandoc is not installed or not in PATH." << std::endl;
        return 1;
    }

    // Construct the pandoc command.
    // Using a base command and adding arguments is safer.
    std::string command = "pandoc \"" + input + "\"" +
                          " -o \"" + output + "\"" +
                          " --pdf-engine=pdflatex" +
                          " -V fontsize=12pt" +
                          " -V documentclass=article" +
                          " -V geometry:margin=1in";

    int ret = system(command.c_str());

    if (ret != 0) {
        std::cerr << "Error: pandoc command failed." << std::endl;
        return 1;
    }

    std::cout << "Successfully converted '" << input << "' to '" << output << "'" << std::endl;

    return 0;
}
