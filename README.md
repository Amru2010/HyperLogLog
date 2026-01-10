# HyperLogLog Implementation

A high-performance C++ implementation of the HyperLogLog algorithm for cardinality estimation, with comparison to exact counting methods.

## Overview

HyperLogLog is a probabilistic data structure used to estimate the cardinality (count of unique elements) in a dataset with minimal memory usage. This implementation demonstrates the trade-off between accuracy and resource consumption by comparing HyperLogLog against exact counting using `std::set`.

## Features

- **HyperLogLog Algorithm**: Efficient cardinality estimation using configurable precision
- **Exact Counting Baseline**: Standard `std::set`-based counting for accuracy comparison
- **Performance Metrics**: Measures execution time and memory usage for both methods
- **Test Data Generator**: Utility to generate large datasets for benchmarking
- **Windows Memory Tracking**: Platform-specific memory profiling using Windows API

## Files

- **`hll.cpp`**: Main implementation comparing HyperLogLog vs exact counting
- **`fill.cpp`**: Test data generator that creates `input.txt` with random integers
- **`input.txt`**: Generated dataset (10 million rows by default)

## Algorithm Details

### Pseudocode & Logic
The implementation follows the canonical HyperLogLog algorithm as described in the original paper, using the following logic (adapted for m = 2^16).

**Definitions:**
- Let `h: D -> {0, 1}^32` be a hash function from D to binary 32-bit words.
- Let `ρ(s)` be the position of the leftmost 1-bit of `s`.
- Define constants:
  - `α_16 = 0.673`
  - `α_32 = 0.697`
  - `α_64 = 0.709`
  - `α_m = 0.7213/(1 + 1.079/m)` for `m >= 128`

**Algorithm:**
```text
Program HYPERLOGLOG (input M: multiset of items from domain D)

1. Assume m = 2^b with b in [4..16].
2. Initialize registers M[1], ..., M[m] to 0.
3. For v in M:
     x = h(v)
     j = 1 + <x_1...x_b>_2   (binary address from first b bits)
     w = x_{b+1}x_{b+2}...   (remaining bits)
     M[j] = max(M[j], ρ(w))

4. Raw Estimate E:
     E = α_m * m^2 * (sum_{j=1}^m 2^{-M[j]})^-1

5. Range Corrections:
     # Small Range
     If E <= (5/2) * m:
         Let V be the number of registers equal to 0.
         If V != 0:
             E* = m * log(m/V)
         Else:
             E* = E

     # Intermediate Range
     If (5/2) * m < E <= (1/30) * 2^32:
         E* = E

     # Large Range
     If E > (1/30) * 2^32:
         E* = -2^32 * log(1 - E/2^32)

6. Return estimate E*
```

_Note: The actual C++ implementation uses `splitmix64` (a 64-bit hash) and `__builtin_clzll` for performance on 64-bit systems, but follows the logic above including the range corrections derived for 32-bit HLL. Since the large range correction is due to hash collision which is very rare for 64-bit hash thus we can ignore the hash correction for large range in this case._

### Parameters
- **Precision**: `p = 16` (65,536 buckets)
- **Hash Function**: `splitmix64` (64-bit)
- **Memory**: ~64 KB for registers

### Exact Counting
- **Data Structure**: `std::set<string>`
- **Memory**: Proportional to number of unique elements
- **Accuracy**: 100% (ground truth)

## Building

### Prerequisites
- C++ compiler with C++11 support (MSVC, GCC, or Clang)
- Windows OS (for memory profiling functions)

### Compilation
```bash
# Compile the main HyperLogLog comparison
g++ hll.cpp -o hll.exe

# Compile the test data generator
g++ fill.cpp -o fill.exe
```

## Usage

### 1. Generate Test Data
First, create a test dataset:
```bash
./fill.exe
```
This generates `input.txt` with 10 million integers (approximately 1 million unique values).

### 2. Run Comparison
Execute the HyperLogLog benchmark:
```bash
./hll.exe
```

### Sample Output (For p = 16)
```
Exact (std::set)
  Count   : 999963
  Time    : 20085 ms
  Memory  : 81580 KB

HyperLogLog
  Estimate: 1000217
  Error: 0.0254655 %
  Time    : 693 ms
  Memory  : 76 KB
```

## Configuration

### Adjusting Precision
Modify the precision parameter `p` in `hll.cpp`:
```cpp
int p = 16;  // Valid range: 4-16
```

| p Value | Buckets (m) | Memory | Standard Error |
|---------|-------------|--------|----------------|
| 10      | 1,024       | ~1 KB  | 3.2%           |
| 12      | 4,096       | ~4 KB  | 1.6%           |
| 14      | 16,384      | ~16 KB | 0.8%           |
| 15      | 32,768      | ~32 KB | 0.57%          |
| 16      | 65,536      | ~64 KB | 0.4%           |

### Adjusting Test Data Size
Modify `ROWS` in `fill.cpp` (line 7):
```cpp
const int ROWS = 10'000'000;  // Adjust as needed
```

## Performance Characteristics

### Time Complexity
- **HyperLogLog**: O(n) with very low constant factor
- **Exact Counting**: O(n log k) where k = unique elements

### Space Complexity
- **HyperLogLog**: O(m) where m = 2^p (constant)
- **Exact Counting**: O(k) where k = unique elements (linear)

## Technical Implementation

### Hash Function
Uses `splitmix64`, a fast and high-quality 64-bit hash mixer:
- Ensures uniform distribution across buckets
- Reduces collisions and bias
- Combines with `std::hash<string>` for string hashing

### Leading Zero Counting
Utilizes GCC/Clang built-in `__builtin_clzll()` (64-bit) for efficient leading zero detection.

### Memory Profiling
Windows-specific implementation using:
- `GetProcessMemoryInfo()` from `psapi.h`
- Reports private working set (not shared memory)

## Limitations

- **Platform**: Memory tracking is Windows-specific (requires modification for Linux/macOS)
- **Small Cardinalities**: HyperLogLog performs poorly for very small datasets (<100 unique elements)

## Future Enhancements

- [ ] Cross-platform memory profiling
- [ ] Multi-threading support for parallel bucket updates
- [ ] Command-line parameter configuration

## References

- [Original HyperLogLog Paper](http://algo.inria.fr/flajolet/Publications/FlFuGaMe07.pdf) by Flajolet et al. This is a great paper, I highly recommend this to you all, gives great insight.