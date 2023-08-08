#include <iostream>
#include <cassert>
#include <string>
#include <vector>

#define LONG_STRING "new looooooooooooooong string"

int main() {
    constexpr int NUMBER = 10000000;
    std::vector<std::string>bg;
    for (int i = 0; i < NUMBER; ++i) {
        bg.push_back(LONG_STRING);
    }
    std::cout << bg.size() << std::endl;
    for (auto& it : bg) {
        assert(it == LONG_STRING);
    }
    return 0;
}