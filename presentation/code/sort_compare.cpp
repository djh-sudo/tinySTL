#include <cstdlib>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>

using namespace std;
using namespace chrono;

inline int cmp(const void* x, const void* y) {return *(int*)x - *(int*)y; }

const long long BIG_MOD = 1000000000;

fstream qsort_log, intro_log;

void compare(long long number) {
    vector<long long>quick_sort;
    for (long long i = 0; i < number; ++i) { quick_sort.push_back(rand() % BIG_MOD); }
    vector<long long>intro_sort = quick_sort;
    // quick sort
    auto start1 = system_clock::now();
    qsort(&quick_sort.front(), number, sizeof(long long), cmp);
    auto end1 = system_clock::now();
    qsort_log << double(duration_cast<microseconds>(end1 - start1).count()) << endl;
    // std sort
    auto start2 = system_clock::now();
    std::sort(quick_sort.begin(), quick_sort.end());
    auto end2 = system_clock::now();
    intro_log << double(duration_cast<microseconds>(end2 - start2).count()) << endl;
}

int main() {
    srand(time(NULL));
    long long NUMBER = 10000000;
    

    qsort_log.open("./msvc-qsort-log.log", ios::out);
    intro_log.open("./msvc-log.log", ios::out);

    for (long long i = 10000; i <= NUMBER; i *= 10) { compare(i); }
    
    qsort_log.close();
    intro_log.close();
    return 0;
}