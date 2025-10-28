# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a Docker container image specifically designed for AtCoder competitive programming. It provides a comprehensive development environment with multiple programming languages and their libraries optimized for competitive programming.

## Container Variants

This repository provides multiple container variants optimized for different use cases. Both versions use multi-stage builds for optimized image size and security.

- **Full Version** (`Dockerfile`): Complete environment with all languages and libraries
  - Size: ~6-7GB
  - Includes: **All 9 languages** (Python, Node.js, Java, Ruby, Erlang, Elixir, Rust, C++, PHP)
  - Scientific libraries: NumPy, SciPy, PyTorch, pandas, scikit-learn
  - Optimization: or-tools
  - C++ libraries: Boost 1.83, LibTorch 2.8.0
  - Best for: Complete competitive programming environment with ML libraries

- **Lite Version** (`Dockerfile.lite`): Lightweight, optimized for CI/CD
  - Size: ~3.5-3.6GB
  - Includes: **All 9 languages** (Python, Node.js, Java, Ruby, Erlang, Elixir, Rust, C++, PHP)
  - Core competitive programming libraries only (no ML/scientific libraries)
  - Best for: CI pipelines, quick testing, dev containers

## Build Commands

```bash
# Build the Full version
docker build -t atcoder-container:latest .

# Build the Lite version
docker build -t atcoder-container:lite -f Dockerfile.lite .

# Build with specific tag
docker build -t atcoder-container:2025 .
```

## Supported Languages and Versions

The container includes the following languages (versions as of January 2025):

### Full Version (Dockerfile)
- **Python** 3.13.7 (with LTO and BOLT optimizations on x86_64)
- **Node.js** 22.19.0
- **Java** OpenJDK 23.0.1
- **Ruby** 3.4.5
- **Erlang/OTP** 28.0.2
- **Elixir** 1.18.4 (using OTP 27 binary for OTP 28 compatibility)
- **Rust** 1.87.0
- **C++** GCC 13 (g++-13) on Ubuntu 24.04
- **PHP** 8.4.12 (with JIT compiler)

### Lite Version (Dockerfile.lite)
- Same language versions as Full version
- **No scientific computing libraries** (NumPy, SciPy, PyTorch, pandas, scikit-learn)
- **No optimization libraries** (or-tools)
- **No advanced C++ libraries** (Boost, LibTorch)

**Note**: All version information is defined in `toml/*.toml` configuration files, which serve as the single source of truth for language versions.

## Key Architecture Decisions

### Build Strategy
**Both versions use multi-stage builds** for optimized image size and security:
- Builder stage: Compiles languages from source with all development dependencies
- Runtime stage: Only includes runtime dependencies and built artifacts

**Differences**:
- Full version: Includes ML libraries (NumPy, SciPy, PyTorch), optimization libraries (or-tools), advanced C++ libraries (Boost, LibTorch)
- Lite version: Core competitive programming libraries only

### Language Installation Methods
- **Python**: Built from source with PGO (Profile-Guided Optimization) and optimization flags
- **Node.js**: Precompiled binary installation from nodejs.org
- **Java**: OpenJDK precompiled binary from java.net
- **Ruby**: Built using ruby-build (standard build, no GC patch)
- **Erlang/Elixir**: Built from source
- **Rust**: Installed from official precompiled tarball (not asdf)
- **C++**: System package (g++-13 from Ubuntu 24.04 repositories)

### Competitive Programming Libraries
- **Python**: ac-library-python, sortedcontainers, NumPy, SciPy, PyTorch CPU (full version)
- **Java**: ac-library-java v2.0.0 (ac_library.jar)
- **Ruby**: ac-library-rb, various gems for algorithms
- **JavaScript**: ac-library-js, data-structure-typed, mathjs
- **C++**: AC Library 1.6, Boost 1.83, Eigen3 3.4.0 (full version)
- **All languages**: online-judge-tools (oj) and atcoder-cli

### Essential Tools
- **jq** v1.7: JSON processor - required for makefile task URL parsing
- **online-judge-tools (oj)**: Test case download and automated submission
- **atcoder-cli (acc)**: Contest setup and management
- **git**, **curl**, **wget**: Version control and HTTP clients
- **build-essential**: Compilation tools (gcc, g++, make)

### Special Configurations
- **Java execution**: Uses `/judge/java.sh` wrapper to set stack size dynamically
- **Elixir**: Pre-built Mix release at `/judge/main` with EXLA and Nx dependencies
- **Python**: Includes scientific computing stack (NumPy, Pandas, SciPy, etc.) in full version
- **Architecture support**: x86_64 and aarch64 (ARM64) with conditional builds
- **Optimization**: ccache enabled for faster recompilation

## File Structure for Language Updates

When updating language versions or adding dependencies:
- `toml/*.toml`: **Primary source of truth** for language versions and configurations
  - `toml/cpython.toml`: Python version and packages
  - `toml/js-node.toml`: Node.js version and npm packages
  - `toml/ruby.toml`: Ruby version and gems
  - `toml/erlang.toml`: Erlang/OTP version
  - `toml/elixir-nx.toml`: Elixir version and Mix dependencies
  - `toml/config.toml`: C++ compiler and library versions
- `python/freeze.txt`: Python package list (full version)
- `java/java.sh`: Java execution wrapper script
- `elixir/`: Elixir project files (mix.exs, config.exs, main.ex)
- `rust/Cargo-lite.toml`: Rust dependencies for lite version

## Version Verification

To verify that language versions in this documentation match the actual configuration:

```bash
# Check versions in Dockerfile
grep -E "AC_CPYTHON_VERSION|NODE_VERSION|AC_OTP_VERSION|RUST_VERSION" Dockerfile
grep -E "ruby-build|openjdk|elixir|php" Dockerfile

# Check versions in Dockerfile.lite
grep -E "AC_CPYTHON_VERSION|NODE_VERSION|AC_OTP_VERSION|RUST_VERSION" Dockerfile.lite
grep -E "ruby-build|openjdk|elixir|php" Dockerfile.lite

# Verify PHP is in both versions
grep -i "php" Dockerfile        # Should show PHP 8.4.12
grep -i "php" Dockerfile.lite   # Should show PHP 8.4.12
```

**Note**: The single source of truth for versions is in `Dockerfile` and `Dockerfile.lite`. This documentation should be updated whenever language versions are changed.

## Important Notes

- Base image: Ubuntu 24.04
- Timezone: Asia/Tokyo
- Locale: ja_JP.UTF-8
- Environment variable `ATCODER=1` is set
- The container is optimized for AtCoder's judge environment
- Version information in this file should be kept in sync with `toml/*.toml` files
- C++ libraries (Boost, Eigen, OR-Tools) are included in full version, optional in lite version
