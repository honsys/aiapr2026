#include <iostream>
#include <string>
class Greeter {
public:
    void hello(std::string name) {
        std::cout << "Hello, " << name << std::endl;
    }
};
int main() {
    Greeter g;
    g.hello("C++");
    return 0;
}
