#include "converter.h"

int main(int argc, char* argv[]) {
    if (argc != 4 || std::string(argv[2]) != "-o") {
        std::cerr << "Usage: " << argv[0] << " <input-file> -o <output-file>" << std::endl;
        return 1;
    }

    std::string input = argv[1];
    std::string output = argv[3];

    try {
        convert_file(input, output);
        std::cout << "Successfully converted '" << input << "' to '" << output << "'" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
