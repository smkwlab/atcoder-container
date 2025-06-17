# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a Docker container image specifically designed for AtCoder competitive programming. It provides a comprehensive development environment with multiple programming languages and their libraries optimized for competitive programming.

## Build Commands

```bash
# Build the Docker image
docker build -t atcoder-container:latest .

# Build with specific tag
docker build -t atcoder-container:2025 .
```

## Supported Languages and Versions

The container includes the following languages (as of the latest Dockerfile):
- **Python** 3.13.5 (with LTO and BOLT optimizations on x86_64)
- **Node.js** 22.16.0
- **Java** OpenJDK 23.0.1
- **Ruby** 3.4.4 (with GC patch)
- **Erlang/OTP** 28.0
- **Elixir** 1.18.4 (using OTP 27 binary for OTP 28 compatibility)
- **Rust** 1.70.0
- **C++** g++ 12.3.0

## Key Architecture Decisions

### Language Installation Methods
- **Python**: Built from source with optimization flags
- **Node.js**: Direct binary installation from nodejs.org
- **Java**: OpenJDK binary from java.net
- **Ruby**: Built using ruby-build
- **Erlang/Elixir**: Built from source
- **Rust**: Installed via asdf
- **C++**: System package (g++-12)

### Competitive Programming Libraries
- **Python**: ac-library-python, sortedcontainers, NumPy, SciPy, PyTorch CPU
- **Java**: ac-library-java v2.0.0 (ac_library.jar)
- **Ruby**: ac-library-rb, various gems for algorithms
- **JavaScript**: ac-library-js, data-structure-typed, mathjs
- **All languages**: online-judge-tools (oj) and atcoder-cli

### Special Configurations
- **Java execution**: Uses `/judge/java.sh` wrapper to set stack size dynamically
- **Elixir**: Pre-built Mix release at `/judge/main` with EXLA and Nx dependencies
- **Python**: Includes scientific computing stack (NumPy, Pandas, SciPy, etc.)
- **Architecture support**: x86_64 and aarch64 (ARM64) with conditional builds

## File Structure for Language Updates

When updating language versions or adding dependencies:
- `python/freeze.txt`: Python package list
- `java/java.sh`: Java execution wrapper script
- `elixir/`: Elixir project files (mix.exs, config.exs, main.ex)
- `toml/`: Configuration files for various build tools

## Important Notes

- Base image: Ubuntu 24.04
- Timezone: Asia/Tokyo
- Locale: ja_JP.UTF-8
- Environment variable `ATCODER=1` is set
- The container is optimized for AtCoder's judge environment
- Some C++ libraries (Boost, Eigen) are commented out in the Dockerfile but can be enabled if needed