#include <iostream>
#include <fstream>
#include <random>
#include <unordered_set>

int main() {
    const int ROWS = 10'000'000;

    std::ofstream out("input.txt");
    if (!out.is_open()) {
        std::cerr << "Failed to open input.txt\n";
        return 1;
    }

    // Random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 1'000'000);

    std::unordered_set<int> uniqueValues;
    uniqueValues.reserve(ROWS);   // improve performance

    for (int i = 0; i < ROWS; ++i) {
        int value = dist(gen);
        out << value << '\n';
        uniqueValues.insert(value);
    }

    out.close();

    std::cout << "Total rows written: " << ROWS << std::endl;
    std::cout << "Unique rows inserted: " << uniqueValues.size() << std::endl;

    return 0;
}
