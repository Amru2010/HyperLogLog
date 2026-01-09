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

        // First p bits = bucket
        int bucket = h >> (64 - p);

        // Remaining bits
        uint64_t w = h << p;

        // rho(w): position of first 1-bit
        int rho = __builtin_clzll(w) + 1;

        M[bucket] = max(M[bucket], (uint8_t)rho);
    }

    // Raw estimate
    double sum = 0.0;
    int V = 0;
    for (int i = 0; i < m; i++) {
        sum += pow(2.0, -M[i]);
        if (M[i] == 0) V++;
    }

    double alpha;
    if (m == 16) alpha = 0.673;
    else if (m == 32) alpha = 0.697;
    else if (m == 64) alpha = 0.709;
    else alpha = 0.7213 / (1.0 + 1.079 / m);

    double E = alpha * m * m / sum;
    double E_star = E;

    // ---- Small range correction ----
    if (E <= 2.5 * m) {
        if (V != 0)
            E_star = m * log((double)m / V);
    }
    // ---- Large range correction ----
    else if (E > (1.0 / 30.0) * pow(2.0, 32)) {
        E_star = -pow(2.0, 32) * log(1.0 - E / pow(2.0, 32));
    }
    // ---- Intermediate range: no correction ----

    auto end = high_resolution_clock::now();
    time_ms = duration_cast<milliseconds>(end - start).count();
    mem_kb = getPrivateMemoryKB();

    return E_star;
}

/* ------------------ MAIN ------------------ */
int main() {
    const string filename = "input.txt";

    long long time_exact, time_hll;
    size_t mem_exact, mem_hll;

    size_t exact = exactCount(filename, time_exact, mem_exact);

    int p = 16;
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
