# Cache Simulator

A self project implementing an **L1 cache simulator in C++** for quad-core processors with **MESI cache coherence** support. The simulator models realistic multi-core cache behavior and provides detailed performance statistics across configurable cache parameters.

## Overview

This project simulates the L1 cache subsystem of a quad-core processor, focusing on how caches interact with each other through a shared bus and maintain coherence using the **MESI (Modified, Exclusive, Shared, Invalid)** protocol. It's designed to help analyze how cache design choices — like set count and block size — affect real-world performance metrics such as miss rate and bus traffic.

## Features

- **MESI Cache Coherence Protocol** — Full support for Modified, Exclusive, Shared, and Invalid states across all four cores to ensure data consistency in a multi-core environment.
- **Write-Back & Write-Allocate Policies** — Realistic handling of writes, deferring memory updates until eviction and allocating cache lines on write misses.
- **LRU Replacement Policy** — Least Recently Used eviction strategy for managing cache line replacement within sets.
- **Configurable Cache Parameters** — Easily vary the number of sets and block size to explore different cache configurations.
- **Performance Analysis** — Tracks and reports key metrics:
  - Cache miss rates
  - Bus transactions
  - Bus traffic (bytes transferred)
  - Write-backs to memory

## Motivation

Cache coherence and replacement policy decisions have a major impact on multi-core system performance. This simulator was built as a self-directed project to deepen understanding of:
- How cache coherence protocols (MESI) prevent data hazards across cores
- The trade-offs between different cache configurations
- How bus contention and traffic scale with cache design choices

## Tech Stack

- **Language:** C++

## Getting Started

### Prerequisites
- A C++ compiler (e.g., g++ or clang++) supporting C++11 or later

### Build
```bash
g++ -o cache_simulator main.cpp -std=c++11
```

### Run
```bash
./cache_simulator <config_options>
```
*(Update the above with your actual executable name and command-line arguments.)*

## Example Configurations Analyzed

| Parameter    | Values Tested |
|--------------|---------------|
| Number of Sets | Varied across multiple configurations |
| Block Size     | Varied across multiple configurations |

For each configuration, the simulator reports:
- Miss rate per core
- Total bus transactions
- Total bus traffic
- Number of write-backs

## Key Learnings

- Implementing and debugging a hardware-level coherence protocol (MESI) in software
- Understanding the interplay between cache size, associativity, and block size on performance
- Quantifying the cost of coherence traffic on a shared bus in multi-core systems

## Future Improvements

- Add support for additional coherence protocols (e.g., MOESI)
- Extend to more than 4 cores
- Add a visualization layer for cache state transitions
- Support additional replacement policies (e.g., FIFO, Random)

## License

This project was built for personal learning and portfolio purposes.
