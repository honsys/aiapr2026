#pragma once
#include <string>

namespace www {

class Redirector {
public:
    Redirector(std::string target_url, int port);
    bool run();

private:
    std::string target_url_;
    int port_;
};

} // namespace www
