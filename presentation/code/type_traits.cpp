#include <type_traits>
#include <iostream>

using namespace std;

class CXX {
public:
    CXX() {};
public:
    int m_a;
    int m_b;
    int m_c;
};

int main() {
    std::cout << is_pod<CXX>() << std::endl;
    std::cout << is_trivially_constructible<CXX>() << std::endl;
    std::cout << is_trivially_copyable<CXX>() << std::endl;
    std::cout << is_trivially_destructible<CXX>() << std::endl;
    // ...
    return 0;
}