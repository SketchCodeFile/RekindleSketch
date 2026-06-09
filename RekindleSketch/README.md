# RekindleSketch

A memory-efficient sketch data structure for detecting recent persistent flows.

## Overview

RekindleSketch is a time-aware sketch-based data structure for detecting recent persistent flows that remain continuously active within the latest `R` windows. It introduces a memory score mechanism to enable recent persistent flows to be progressively reinforced while obsolete historical activity is quickly forgotten. Based on this mechanism, RekindleSketch further employs a probabilistic replacement strategy to preserve recent persistent flows under hash collisions and efficiently evict inactive flows, achieving high accuracy with compact memory usage.

## Features

- **Memory-efficient**: Configurable memory constraints with automatic bucket/cell calculation
- **Time-aware scoring**: Memory-score-based tracking with decay and reward mechanisms for recent persistent flows
- **Configurable parameters**: Adjustable decay coefficients, reward functions, and thresholds
- **Folder-based input**: Processes all `.dat` files in a folder, with one file corresponding to one window

## Requirements

- C++17 or later
- CMake 3.15+
- Compiler: GCC 7+, Clang 5+, MSVC 2017+

## Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Windows (Visual Studio)

```bash
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

## Configuration

Edit `parm.h` to configure:


| Parameter             | Default | Description                                            |
| --------------------- | ------- | ------------------------------------------------------ |
| `ESTIMATED_CELL_SIZE` | 12      | Estimated memory per cell (bytes, with struct padding) |
| `TOTAL_MEMORY_BYTES`  | 200KB   | Total memory budget                                    |
| `DEFAULT_M`           | auto    | Number of hash buckets (calculated from memory)        |
| `DEFAULT_N`           | 55      | Cells per bucket                                       |
| `DEFAULT_ALPHA`       | 0.015   | Exponential decay factor                               |
| `DEFAULT_BETA`        | 1.5     | Reward coefficient                                     |
| `R`                   | 500     | Number of recent windows to track                      |
| `DALTA`               | 300.0   | Persistence threshold                                  |
| `S`                   | 100     | Query interval in windows                              |
| `TOTAL_WINDOWS`       | 0       | Windows to process (`0` = all)                        |
| `DATA_FOLDER_PATH`    | `""`    | Default path; override via command line              |

## Input Format

- `DATA_FOLDER_PATH` should point to a directory containing one or more `.dat` files.
- Each `.dat` file represents one window.
- Each file is read in binary mode.
- Every 32-bit value in a `.dat` file is treated as one flow ID.
- Files are processed in lexicographic order.
- If a file size is not a multiple of 4 bytes, trailing incomplete bytes are ignored.

## Usage

```bash
# Run with data folder path as argument
./RekindleSketch /path/to/dat/files

# Or set a default path in parm.h and run without arguments
./RekindleSketch
```

## Project Structure

```
.
├── CMakeLists.txt          # Build configuration
├── RekindleSketch.h        # Main sketch class
├── parm.h                  # Tunable parameters
├── MurmurHash3.h           # MurmurHash3 implementation
├── main.cpp                # Entry point
├── README.md
└── LICENSE
```

## Algorithm

RekindleSketch uses:

- **Memory score**: Track flow activity with exponential decay
- **Time-awareness**: Detect recent persistent flows over the latest `R` windows
- **Probabilistic replacement**: Handle hash collisions with replacement probability

## License

MIT License