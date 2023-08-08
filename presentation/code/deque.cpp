#include <cassert>
#include <deque>

using namespace std;

int main() {
    constexpr int NUMBER = 1000000;
    
    deque<int>dq;
    
    for (int i = 0; i < NUMBER; ++i) {
        dq.push_back(i);
    }

    deque<int>::iterator it = dq.begin();
    for (int i = 0; i < NUMBER; ++i) {
        assert(*it == i);
        ++it;
    }
    return 0;
}