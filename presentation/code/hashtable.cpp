#include <fstream>
#include <unordered_set>
#include <vector>

using namespace std;

int main() {
    constexpr int NUMBER = 1000000;
    vector<double>factor;
    vector<int>bucket_count;

    unordered_set<int>s;
    for (int i = 0; i < NUMBER; ++i) {
        s.insert(i);
        factor.push_back(s.load_factor());
        bucket_count.push_back(s.bucket_count());
    }
    // write to code
    ofstream factor_log, bucket_count_log;
    factor_log.open("./factor.log", ios::out);
    bucket_count_log.open("./bucket_count.log", ios::out);
    for (int i = 0; i < NUMBER; ++i) {
        factor_log << factor[i] << endl;
        bucket_count_log << bucket_count[i] << endl;
    }
    factor_log.close();
    bucket_count_log.close();
    return 0;
}