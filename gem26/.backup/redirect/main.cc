#include "www.h"
#include <iostream>
#include <string>
#include <format>

#ifdef __has_include
#  if __has_include(<print>)
#    include <print>
#    define USE_STD_PRINT 1
#  else
#    define USE_STD_PRINT 0
#  endif
#else
#  define USE_STD_PRINT 0
#endif

int main(int argc, char* argv[]) {
    if (argc < 2) {
        if constexpr (USE_STD_PRINT) {
            std::print("Usage: {} <target_url> [port]\\n", argv[0]);
        } else {
            std::cout << std::format("Usage: {} <target_url> [port]\\n", argv[0]);
        }
        return 1;
    }

    std::string target_url = argv[1];
    int port = (argc > 2) ? std::stoi(argv[2]) : 8080;

    www::Redirector redirector(target_url, port);
    if (!redirector.run()) {
        return 1;
    }

    return 0;
}
