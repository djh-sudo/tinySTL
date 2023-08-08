#include <vector>
#include <fstream>

using namespace std;


int main() {
    constexpr int NUMBER = 1000000;
    // log code
    vector<int>v_size(NUMBER, 0);
    vector<int>v_cap(NUMBER, 0);
    // test code
    vector<int>test;
    for (int i = 0; i < NUMBER; ++i) {
        test.push_back(i);
        v_size[i] = test.size();
        v_cap[i] = test.capacity();
    }
    // write to file

    ofstream size_log, cap_log;
    size_log.open("./size.log", ios::out);
    cap_log.open("./cap.log", ios::out);
    for (int i = 0; i < NUMBER; ++i) {
        size_log << v_size[i] << endl;
        cap_log << v_cap[i] << endl;
    }
    size_log.close();
    cap_log.close();
    return 0;
}