#include <bits/stdc++.h>
#include <chrono>
#include <windows.h>
#include <psapi.h>

using namespace std;
using namespace std::chrono;

/* ------------------ HASH ------------------ */
static inline uint64_t splitmix64(uint64_t x) {
    x += 0x9e3779b97f4a7c15;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
    x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
    return x ^ (x >> 31);
}

/* ------------------ MEMORY (WINDOWS) ------------------ */
size_t getPrivateMemoryKB() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(
        GetCurrentProcess(),
        (PROCESS_MEMORY_COUNTERS*)&pmc,
        sizeof(pmc)
    );
    return pmc.PrivateUsage / 1024; // KB
}

/* ------------------ EXACT COUNT ------------------ */
size_t exactCount(
    const string& filename,
    long long& time_ms,
    size_t& mem_kb
) {
    auto start = high_resolution_clock::now();

    set<string> sett;
    ifstream file(filename);
    string s;
    while (getline(file, s)) {
        sett.insert(s);
    }

    auto end = high_resolution_clock::now();
    time_ms = duration_cast<milliseconds>(end - start).count();
    mem_kb = getPrivateMemoryKB();

    return sett.size();
}

/* ------------------ HLL COUNT ------------------ */
double hllCount(
    const string& filename,
    int p,
    long long& time_ms,
    size_t& mem_kb
) {
    auto start = high_resolution_clock::now();

    const int m = 1 << p;
    vector<uint8_t> M(m, 0);

    ifstream file(filename);
    string s;
    while (getline(file, s)) {
        uint64_t h = splitmix64(hash<string>{}(s));
        int bucket = h >> (64 - p);
        uint64_t w = h << p;
        int rho = __builtin_clzll(w) + 1;
        M[bucket] = max(M[bucket], (uint8_t)rho);
    }

    double sum = 0.0;
    for (int i = 0; i < m; i++)
        sum += pow(2.0, -M[i]);

    double alpha = 0.7213 / (1.0 + 1.079 / m);
    double estimate = alpha * m * m / sum;

    auto end = high_resolution_clock::now();
    time_ms = duration_cast<milliseconds>(end - start).count();
    mem_kb = getPrivateMemoryKB();

    return estimate;
}

/* ------------------ MAIN ------------------ */
int main() {
    const string filename = "input.txt";

    long long time_exact, time_hll;
    size_t mem_exact, mem_hll;

    size_t exact = exactCount(filename, time_exact, mem_exact);

    int p = 15;
    double estimate = hllCount(filename, p, time_hll, mem_hll);

    double accuracy = abs(estimate - exact) / exact * 100.0;

    cout << "Exact (std::set)\n";
    cout << "  Count   : " << exact << "\n";
    cout << "  Time    : " << time_exact << " ms\n";
    cout << "  Memory  : " << mem_exact << " KB\n\n";

    cout << "HyperLogLog\n";
    cout << "  Estimate: " << (long long)estimate << "\n";
    cout << "  Accuracy: " << accuracy << " %\n";
    cout << "  Time    : " << time_hll << " ms\n";
    cout << "  Memory  : " << mem_hll << " KB\n";
}
