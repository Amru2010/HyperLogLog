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

### HyperLogLog
- **Hash Function**: Uses `splitmix64` for uniform distribution
- **Precision Parameter**: `p = 15` (32,768 buckets)
- **Estimator**: Standard HyperLogLog estimator with alpha correction
- **Memory**: ~32 KB for p=15 plus some additional space
- **Accuracy**: Typically <1% error on large datasets

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

### Sample Output (For p = 15)
```
Exact (std::set)
  Count   : 999963
  Time    : 21260 ms
  Memory  : 83284 KB

HyperLogLog
  Estimate: 997045
  Accuracy: 0.291721 %
  Time    : 906 ms
  Memory  : 2276 KB
```

## Configuration

### Adjusting Precision
Modify the precision parameter `p` in `hll.cpp` (line 95):
```cpp
int p = 15;  // Valid range: 4-16
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
Utilizes GCC/Clang built-in `__builtin_clzll()` for efficient leading zero detection.

### Memory Profiling
Windows-specific implementation using:
- `GetProcessMemoryInfo()` from `psapi.h`
- Reports private working set (not shared memory)

## Limitations

- **Platform**: Memory tracking is Windows-specific (requires modification for Linux/macOS)
- **Hash Collisions**: Uses standard library hash which may not be cryptographically secure
- **Small Cardinalities**: HyperLogLog performs poorly for very small datasets (<100 unique elements)
- **No Bias Correction**: Doesn't implement small/large range corrections for extreme cardinalities

## Future Enhancements

- [ ] Cross-platform memory profiling
- [ ] Multi-threading support for parallel bucket updates
- [ ] Command-line parameter configuration

## References

- [Original HyperLogLog Paper](http://algo.inria.fr/flajolet/Publications/FlFuGaMe07.pdf) by Flajolet et al. This is a great paper, I highly recommend this to you all, gives great insight.