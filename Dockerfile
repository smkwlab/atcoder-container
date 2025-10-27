# AtCoder Container - Full Version (Multi-stage build)
# Stage 1: Build stage with all development tools
FROM ubuntu:24.04 AS builder

ARG HOME=/home/runner
ARG PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
ARG DEBIAN_FRONTEND=noninteractive
ARG DEBCONF_NONINTERACTIVE_SEEN=true

ENV TZ=Asia/Tokyo

# Install all build dependencies in one layer
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
            curl \
            git \
            ca-certificates \
            locales \
            build-essential \
            g++-13 \
            wget \
            unzip \
            # Python build dependencies
            gdb \
            lcov \
            pkg-config \
            libbz2-dev \
            libffi-dev \
            libgdbm-dev \
            libgdbm-compat-dev \
            liblzma-dev \
            libncurses5-dev \
            libreadline6-dev \
            libsqlite3-dev \
            libssl-dev \
            lzma \
            lzma-dev \
            tk-dev \
            uuid-dev \
            zlib1g-dev \
            libgmp-dev \
            libmpfr-dev \
            libmpc-dev \
            # Ruby build dependencies  
            autoconf \
            bison \
            patch \
            libyaml-dev \
            libz3-dev \
            libgeos-dev \
            # Erlang build dependencies
            unixodbc-dev \
            libblas-dev \
            liblapack-dev \
            # C++ libraries
            libeigen3-dev \
            cmake \
            && \
    localedef -f UTF-8 -i ja_JP ja_JP.UTF-8 && \
    rm -rf /var/lib/apt/lists/*

ENV LANG="ja_JP.UTF-8" \
    LANGUAGE="ja_JP:ja" \
    LC_ALL="ja_JP.UTF-8" \
    ATCODER=1

# Install llvm-bolt for Python BOLT optimization (x86_64 only)
RUN if [ "$(uname -m)" = "x86_64" ]; then \
        apt-get update && \
        apt-get install -y --no-install-recommends llvm-bolt && \
        rm -rf /var/lib/apt/lists/*; \
    fi

# Build Python from source (Full version with LTO and architecture-specific BOLT optimizations)
ARG AC_CPYTHON_VERSION=3.13.7
WORKDIR /tmp
RUN if [ "$(uname -m)" = "x86_64" ]; then \
        ln -s /usr/lib/llvm-18/lib/libbolt_rt_instr.a /usr/lib/libbolt_rt_instr.a; \
    fi && \
    wget -q https://www.python.org/ftp/python/${AC_CPYTHON_VERSION}/Python-${AC_CPYTHON_VERSION}.tar.xz && \
    tar xf Python-${AC_CPYTHON_VERSION}.tar.xz && \
    cd Python-${AC_CPYTHON_VERSION} && \
    if [ "$(uname -m)" = "x86_64" ]; then \
        BOLT_FLAG="--enable-bolt"; \
    else \
        BOLT_FLAG=""; \
    fi && \
    ./configure --enable-optimizations --with-lto=full --with-strict-overflow $BOLT_FLAG --prefix=/opt/python && \
    make -j$(nproc) && \
    make install && \
    cd .. && \
    rm -rf Python-${AC_CPYTHON_VERSION} Python-${AC_CPYTHON_VERSION}.tar.xz

# Install Python packages in build stage
RUN /opt/python/bin/python3.13 -m pip install setuptools==75.8.0

# Install base scientific packages (Full version)
RUN /opt/python/bin/python3.13 -m pip install \
    numpy==2.2.6 \
    pandas==2.3.2 \
    scipy==1.16.1 \
    sympy==1.13.1

# Install additional scientific packages (Full version)
RUN /opt/python/bin/python3.13 -m pip install \
    scikit-learn==1.7.1 \
    numba==0.61.2 \
    polars==1.31.0 \
    PuLP==3.2.2 \
    filelock==3.17.0 \
    fsspec==2025.2.0 \
    Jinja2==3.1.5 \
    joblib==1.4.2 \
    MarkupSafe==3.0.2 \
    threadpoolctl==3.5.0 \
    tzdata==2025.1

# Install competitive programming packages
RUN /opt/python/bin/python3.13 -m pip install \
    "git+https://github.com/not522/ac-library-python@27fdbb71cd0d566bdeb12746db59c9d908c6b5d5" \
    bitarray==3.6.1 \
    more-itertools==10.7.0 \
    sortedcontainers==2.4.0 \
    networkx==3.5 \
    mpmath==1.3.0 \
    six==1.17.0 \
    python-dateutil==2.9.0.post0 \
    pytz==2025.1 \
    typing_extensions==4.12.2

# Install PyTorch CPU and ortools (Full version)
RUN /opt/python/bin/python3.13 -m pip install \
    torch==2.8.0+cpu --index-url https://download.pytorch.org/whl/cpu && \
    /opt/python/bin/python3.13 -m pip install ortools==9.14.6206

# Install online-judge-tools
RUN /opt/python/bin/python3.13 -m pip install online-judge-tools==11.5.1
RUN /opt/python/bin/python3.13 -m pip cache purge

# Build Erlang from source
ARG AC_OTP_VERSION=28.0.2
RUN wget -q -O erlang.tar.gz https://github.com/erlang/otp/releases/download/OTP-${AC_OTP_VERSION}/otp_src_${AC_OTP_VERSION}.tar.gz && \
    mkdir erlang && \
    tar -C erlang --strip-components=1 -xf erlang.tar.gz && \
    cd erlang && \
    ./configure --without-termcap --prefix=/opt/erlang && \
    make -j4 && \
    make install && \
    cd .. && \
    rm -rf erlang erlang.tar.gz

# Build Ruby from source (TOML compliant - no GC patch)
RUN curl -s https://api.github.com/repos/rbenv/ruby-build/releases/latest | \
   grep -o 'https://[^"]*tarball[^"]*' | \
   xargs curl -o ruby-build.tarball -L && \
   tar -xf ruby-build.tarball && \
   PREFIX=/usr/local ./*ruby-build-*/install.sh && \
   ruby-build 3.4.5 /opt/ruby && \
   rm -rf *ruby-build* ruby-build.tarball

# Install Ruby gems
ENV PATH=/opt/ruby/bin:$PATH
RUN gem install -N \
    ac-library-rb:1.2.0 \
    bit_utils:0.1.2 \
    bitarray:1.3.1 \
    fast_trie:0.5.1 \
    faster_prime:1.0.2 \
    immutable-ruby:0.2.0 \
    rbtree:0.4.6 \
    rgl:0.6.6 \
    sorted_containers:1.1.0 \
    sorted_set:1.0.3 && \
    # Fix ac-library-rb gemspec bug (upstream issue: malformed require_paths)
    sed -i 's/lib_helpers\\"/lib_helpers/g' "$(gem environment gemdir)/specifications/ac-library-rb-1.2.0.gemspec"

# Install Rust from official precompiled tarball
ARG RUST_VERSION=1.87.0
RUN case "$(uname -m)" in \
        x86_64) \
            curl "https://static.rust-lang.org/dist/rust-${RUST_VERSION}-x86_64-unknown-linux-gnu.tar.gz" -fO && \
            tar xf "rust-${RUST_VERSION}-x86_64-unknown-linux-gnu.tar.gz" && \
            "./rust-${RUST_VERSION}-x86_64-unknown-linux-gnu/install.sh" --prefix=/opt/rust \
            ;; \
        aarch64) \
            curl "https://static.rust-lang.org/dist/rust-${RUST_VERSION}-aarch64-unknown-linux-gnu.tar.gz" -fO && \
            tar xf "rust-${RUST_VERSION}-aarch64-unknown-linux-gnu.tar.gz" && \
            "./rust-${RUST_VERSION}-aarch64-unknown-linux-gnu/install.sh" --prefix=/opt/rust \
            ;; \
        *) \
            echo "Unsupported architecture: $(uname -m)" \
            exit 1 \
            ;; \
    esac && \
    rm -rf rust-*

# Build Rust project with lightweight libraries
ENV PATH=/opt/rust/bin:$PATH
WORKDIR /tmp/rust-build
RUN mkdir -p .cargo src && \
    echo '[build]' > .cargo/config.toml && \
    echo 'rustflags = ["--cfg", "atcoder"]' >> .cargo/config.toml

# Copy lightweight Rust configuration
COPY rust/Cargo-lite.toml Cargo.toml
COPY rust/Cargo-lite.lock Cargo.lock
RUN echo 'fn main() {}' > src/main.rs && \
    cargo build --release && \
    mkdir -p /opt/rust-project && \
    cp -r target .cargo Cargo.toml Cargo.lock src /opt/rust-project/

# Download AC Library C++ (header-only)
RUN wget -q https://github.com/atcoder/ac-library/archive/refs/tags/v1.6.tar.gz -O ac-library.tar.gz && \
    tar -xzf ac-library.tar.gz && \
    mkdir -p /opt/cpp-headers/include && \
    cp -r ac-library-1.6/atcoder /opt/cpp-headers/include/ && \
    rm -rf ac-library*

# Install Elixir in builder stage
RUN AC_OTP_MAJOR_VERSION=27 && \
    wget -q https://github.com/elixir-lang/elixir/releases/download/v1.18.4/elixir-otp-${AC_OTP_MAJOR_VERSION}.zip && \
    unzip elixir-otp-${AC_OTP_MAJOR_VERSION}.zip 'bin/*' 'lib/*' -d /opt/elixir && \
    rm elixir-otp-${AC_OTP_MAJOR_VERSION}.zip

# Create Elixir project in builder stage
WORKDIR /opt/elixir-project
RUN PATH=/opt/erlang/bin:/opt/elixir/bin:$PATH /opt/elixir/bin/mix local.hex --force && \
    PATH=/opt/erlang/bin:/opt/elixir/bin:$PATH /opt/elixir/bin/mix local.rebar --force && \
    PATH=/opt/erlang/bin:/opt/elixir/bin:$PATH /opt/elixir/bin/mix new main && \
    cd main

COPY elixir/mix.exs /opt/elixir-project/main/mix.exs
RUN mkdir -p /opt/elixir-project/main/config
COPY elixir/config.exs /opt/elixir-project/main/config/config.exs

WORKDIR /opt/elixir-project/main
RUN PATH=/opt/erlang/bin:/opt/elixir/bin:$PATH MIX_ENV=prod /opt/elixir/bin/mix deps.get && \
    PATH=/opt/erlang/bin:/opt/elixir/bin:$PATH MIX_ENV=prod /opt/elixir/bin/mix release && \
    rm -f _build/prod/rel/main/bin/main

# Install LibTorch (Full version - x86_64 only)
# Following ruby.toml specification: install to /usr/local for gem install
# Also copy to /opt/libtorch for Runtime stage
WORKDIR /tmp
ARG AC_LIBTORCH_VERSION="2.8.0"
ENV PATH=/opt/ruby/bin:$PATH
RUN mkdir -p /opt/libtorch/include /opt/libtorch/lib && \
    if [ "$(uname -m)" = "x86_64" ]; then \
        wget -q -O libtorch.zip https://download.pytorch.org/libtorch/cpu/libtorch-shared-with-deps-${AC_LIBTORCH_VERSION}%2Bcpu.zip && \
        unzip -q libtorch.zip && \
        cp -dR libtorch/include /usr/local/ && \
        cp -dR libtorch/lib /usr/local/ && \
        cp -dR libtorch/include /opt/libtorch/ && \
        cp -dR libtorch/lib /opt/libtorch/ && \
        echo /usr/local/lib | tee /etc/ld.so.conf.d/libtorch.conf && \
        ldconfig && \
        rm -rf libtorch*; \
    fi

# Install Rice 4.6.1 before installing the Full version Ruby gems below,
# because or-tools and torch-rb are not compatible with Rice 4.7.0+.
# Note: This step is only performed on x86_64, because the full Ruby gems (including or-tools and torch-rb)
# are only installed on x86_64. On ARM64, these gems are not installed, so Rice is not required.
RUN if [ "$(uname -m)" = "x86_64" ]; then \
        gem install rice:4.6.1; \
    fi

# Install Full version Ruby gems (x86_64 only)
# Following ruby.toml specification: install all gems together with MAKEFLAGS
RUN if [ "$(uname -m)" = "x86_64" ]; then \
        export MAKEFLAGS="-j$(nproc)" && \
        gem install -N \
            ffi-geos:2.5.0 \
            lightgbm:0.4.3 \
            numo-linalg:0.1.7 \
            numo-narray:0.9.2.1 \
            numo-openblas:0.5.1 \
            or-tools:0.16.0 \
            polars-df:0.21.1 \
            rumale:1.0.0 \
            torch-rb:0.21.0 \
            z3:0.0.20230311; \
    fi

# Stage 2: Runtime stage with minimal dependencies
FROM ubuntu:24.04 AS runtime

ARG DEBIAN_FRONTEND=noninteractive
ARG DEBCONF_NONINTERACTIVE_SEEN=true
ENV TZ=Asia/Tokyo

# Install runtime dependencies and essential tools
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
            curl \
            git \
            ca-certificates \
            locales \
            time \
            tzdata \
            xdg-utils \
            wget \
            unzip \
            xz-utils \
            # Runtime libraries only (no -dev packages)
            libssl3t64 \
            libreadline8t64 \
            libsqlite3-0 \
            libffi8 \
            libgdbm6t64 \
            libbz2-1.0 \
            liblzma5 \
            libncurses6 \
            uuid-runtime \
            zlib1g \
            libgmp10 \
            libmpfr6 \
            libmpc3 \
            libyaml-0-2 \
            libz3-4 \
            libgeos3.12.1t64 \
            # Eigen3 runtime (header-only, but some runtime libs)
            libeigen3-dev \
            # Basic build tools for C++ and Rust compilation
            build-essential \
            g++-13 \
            # Boost libraries (Full version)
            libboost-system-dev \
            libboost-filesystem-dev \
            libboost-program-options-dev \
            libboost-regex-dev \
            libboost-graph-dev \
            && \
    localedef -f UTF-8 -i ja_JP ja_JP.UTF-8 && \
    apt-get clean && \
    apt-get autoremove -y && \
    rm -rf /var/lib/apt/lists/*

ENV LANG="ja_JP.UTF-8" \
    LANGUAGE="ja_JP:ja" \
    LC_ALL="ja_JP.UTF-8" \
    ATCODER=1

# Copy built languages from builder stage
COPY --from=builder /opt/python /usr/local/
COPY --from=builder /opt/ruby /opt/ruby
COPY --from=builder /opt/erlang /usr/local/
COPY --from=builder /opt/rust /usr/local/
COPY --from=builder /opt/rust-project /judge/
COPY --from=builder /opt/cpp-headers/include /usr/local/include/
COPY --from=builder /opt/elixir /usr/local/
COPY --from=builder /opt/elixir-project /judge/
COPY --from=builder /home/runner/.mix /root/.mix

# Copy LibTorch (Full version - x86_64 only, but copy empty dir for ARM64)
COPY --from=builder /opt/libtorch /usr/local/libtorch

# Configure LibTorch library path for x86_64 only
# ARM64 doesn't have LibTorch, so we skip configuration
RUN ARCH=$(uname -m) && \
    if [ "$ARCH" = "x86_64" ]; then \
        LIBTORCH_LIB="/usr/local/libtorch/lib"; \
        if [ ! -d "$LIBTORCH_LIB" ]; then \
            echo "ERROR: LibTorch lib directory not found at $LIBTORCH_LIB" >&2; \
            exit 1; \
        fi; \
        if [ -z "$(ls -A "$LIBTORCH_LIB" 2>/dev/null)" ]; then \
            echo "ERROR: LibTorch lib directory is empty at $LIBTORCH_LIB" >&2; \
            exit 1; \
        fi; \
        echo "$LIBTORCH_LIB" > /etc/ld.so.conf.d/libtorch.conf && \
        ldconfig && \
        echo "LibTorch configured successfully for x86_64"; \
    else \
        echo "Skipping LibTorch configuration for $ARCH architecture"; \
    fi

# Set up paths and library paths
ENV PATH=/opt/ruby/bin:/usr/local/bin:$PATH \
    LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH \
    ERL_ROOTDIR=/usr/local/lib/erlang

# Install Node.js (precompiled - lightweight)
ARG NODE_VERSION=22.19.0
RUN case "$(uname -m)" in \
        x86_64) \
            wget -q -O /tmp/node.tar.xz https://nodejs.org/dist/v$NODE_VERSION/node-v$NODE_VERSION-linux-x64.tar.xz \
            ;; \
        aarch64) \
            wget -q -O /tmp/node.tar.xz https://nodejs.org/dist/v$NODE_VERSION/node-v$NODE_VERSION-linux-arm64.tar.xz \
            ;; \
        *) \
            echo "Unsupported architecture: $(uname -m)" \
            exit 1 \
            ;; \
    esac && \
    tar -C /usr/local --strip-components=1 -xf /tmp/node.tar.xz --wildcards '*/bin' '*/share' '*/lib' '*/include' && \
    ln -s /usr/local/lib/node_modules /node_modules && \
    npm install -g \
      ac-library-js@0.1.1 \
      data-structure-typed@2.0.4 \
      immutable@5.1.3 \
      lodash@4.17.21 \
      mathjs@14.7.0 \
      tstl@3.0.0 \
      atcoder-cli@2.2.0

# Install Java (precompiled)
RUN case "$(uname -m)" in \
        x86_64) \
            curl -L https://download.java.net/java/GA/jdk23.0.1/c28985cbf10d4e648e4004050f8781aa/11/GPL/openjdk-23.0.1_linux-x64_bin.tar.gz | \
            tar zx -C /usr/local --strip-components 1 \
            ;; \
        aarch64) \
            curl -L https://download.java.net/java/GA/jdk23.0.1/c28985cbf10d4e648e4004050f8781aa/11/GPL/openjdk-23.0.1_linux-aarch64_bin.tar.gz | \
            tar zx -C /usr/local --strip-components 1 \
            ;; \
        *) \
            echo "Unsupported architecture: $(uname -m)" \
            exit 1 \
            ;; \
    esac

# AC Library for Java
RUN wget -q https://github.com/ocha98/ac-library-java/releases/download/v2.0.0/ac_library23.jar && \
    mv ac_library23.jar ac_library.jar

# Java execution script
COPY java/java.sh /judge/java.sh
RUN chmod +x /judge/java.sh

# Elixir is already copied from builder stage

# Install PHP 8.4.12 from PPA (Full version only)
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y software-properties-common && \
    add-apt-repository -y ppa:ondrej/php && \
    apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
        php8.4-cli \
        php8.4-gmp \
        php8.4-bcmath \
        php8.4-sqlite3 && \
    printf "opcache.enable_cli = 1\nopcache.jit = tracing\nopcache.jit_buffer_size = 128M\n" | tee -a /etc/php/8.4/cli/conf.d/10-opcache.ini > /dev/null && \
    apt-get clean && \
    apt-get autoremove -y && \
    rm -rf /var/lib/apt/lists/*

# C, C++ compiler setup
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 90 --slave /usr/bin/g++ g++ /usr/bin/g++-13

# Create info file
RUN echo 'AtCoder Full Multistage: GCC 13, AC Library C++ 1.6, Eigen3 3.4.0, Boost 1.83, LibTorch 2.8.0, NumPy, SciPy, PyTorch, or-tools, PHP 8.4.12 with JIT' > /usr/local/share/container-info.txt

# Set final working directory
WORKDIR /root