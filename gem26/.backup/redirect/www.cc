#include "www.h"
#include "httplib.h"
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

namespace www {

Redirector::Redirector(std::string target_url, int port)
    : target_url_(std::move(target_url)), port_(port) {}

bool Redirector::run() {
    httplib::Server svr;

    // Use a catch-all route to redirect everything
    svr.Get("/.*", [&](const httplib::Request& req, httplib::Response& res) {
        if constexpr (USE_STD_PRINT) {
            std::print("Redirecting request: {} to {}\\n", req.path, target_url_);
        } else {
            std::cout << std::format("Redirecting request: {} to {}\\n", req.path, target_url_);
        }
        res.set_redirect(target_url_, 302);
    });

    // Also handle root
    svr.Get("/", [&](const httplib::Request& req, httplib::Response& res) {
        if constexpr (USE_STD_PRINT) {
            std::print("Redirecting root to {}\\n", target_url_);
        } else {
            std::cout << std::format("Redirecting root to {}\\n", target_url_);
        }
        res.set_redirect(target_url_, 302);
    });

    if constexpr (USE_STD_PRINT) {
        std::print("Starting redirector on port {} -> {}\\n", port_, target_url_);
    } else {
        std::cout << std::format("Starting redirector on port {} -> {}\\n", port_, target_url_);
    }

    if (!svr.listen("0.0.0.0", port_)) {
        if constexpr (USE_STD_PRINT) {
            std::print("Error: Could not start server on port {}\\n", port_);
        } else {
            std::cerr << std::format("Error: Could not start server on port {}\\n", port_);
        }
        return false;
    }

    return true;
}

} // namespace www
