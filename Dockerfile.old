FROM ubuntu:24.04

# コンテナ構築用にホームディレクトリ代入
ARG HOME=/home/runner
ARG PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
# No asdf needed - using direct installation for all languages

ARG DEBIAN_FRONTEND=noninteractive
ARG DEBCONF_NONINTERACTIVE_SEEN=true
ENV TZ=Asia/Tokyo \
    PATH=$HOME/bin:$PATH

# base system
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
            curl \
            git \
            ca-certificates \
            locales \
            time \
            tzdata \
            xdg-utils \
            g++-13 \
            build-essential \
            wget \
            unzip \
            && \
    localedef -f UTF-8 -i ja_JP ja_JP.UTF-8 && \
    apt-get clean && \
    apt-get autoremove -y && \
    rm -rf /var/lib/apt/lists/*
ENV LANG="ja_JP.UTF-8" \
    LANGUAGE="ja_JP:ja" \
    LC_ALL="ja_JP.UTF-8" \
    ATCODER=1

# No asdf needed - using direct installation for all languages

# Python, oj
ARG AC_CPYTHON_VERSION=3.13.7

# Install build dependencies
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y wget gnupg software-properties-common && \
#    wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
#    add-apt-repository "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-18 main" && \
#    apt-get update && \
    DEBIAN_FRONTEND=noninteractive DEBCONF_NONINTERACTIVE_SEEN=true \
   apt-get install -y \
   build-essential \
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
   $(if [ "$(uname -m)" = "x86_64" ]; then echo "llvm-18"; fi) \
   git \
#   llvm-bolt \
   && rm -rf /var/lib/apt/lists/*

# Build and install Python (architecture-specific optimization)
WORKDIR /tmp
RUN wget -q https://www.python.org/ftp/python/${AC_CPYTHON_VERSION}/Python-${AC_CPYTHON_VERSION}.tar.xz && \
   tar xf Python-${AC_CPYTHON_VERSION}.tar.xz && \
   cd Python-${AC_CPYTHON_VERSION} && \
   case "$(uname -m)" in \
       x86_64) \
           # AMD64: Full optimization (PGO + LTO) for maximum performance \
           ./configure --enable-optimizations --with-lto=full --with-strict-overflow && \
           make -j$(nproc) \
           ;; \
       aarch64) \
           # ARM64: PGO only for faster build time \
           ./configure --enable-optimizations --with-strict-overflow && \
           make -j$(nproc) \
           ;; \
       *) \
           echo "Unsupported architecture: $(uname -m)" \
           exit 1 \
           ;; \
   esac && \
   make altinstall && \
   cd .. && \
   rm -rf Python-${AC_CPYTHON_VERSION} Python-${AC_CPYTHON_VERSION}.tar.xz

# Install Python packages
COPY python/freeze.txt /tmp/freeze.txt

# Install setuptools first
RUN python3.13 -m pip install setuptools==75.8.0

# Install base scientific packages
RUN python3.13 -m pip install \
    numpy==2.2.6 \
    pandas==2.3.2 \
    scipy==1.16.1 \
    sympy==1.13.1

# Install additional scientific packages
RUN python3.13 -m pip install \
    networkx==3.5 \
    scikit-learn==1.7.1 \
    numba==0.61.2 \
    mpmath==1.3.0

# Install the rest of the packages
RUN python3.13 -m pip install \
    "git+https://github.com/not522/ac-library-python@27fdbb71cd0d566bdeb12746db59c9d908c6b5d5" \
    bitarray==3.6.1 \
    filelock==3.17.0 \
    fsspec==2025.2.0 \
    Jinja2==3.1.5 \
    joblib==1.4.2 \
    MarkupSafe==3.0.2 \
    more-itertools==10.7.0 \
    polars==1.31.0 \
    PuLP==3.2.2 \
    python-dateutil==2.9.0.post0 \
    pytz==2025.1 \
    six==1.17.0 \
    sortedcontainers==2.4.0 \
    threadpoolctl==3.5.0 \
    typing_extensions==4.12.2 \
    tzdata==2025.1

# Install PyTorch CPU and ortools
RUN python3.13 -m pip install \
    torch==2.6.0+cpu --index-url https://download.pytorch.org/whl/cpu && \
    python3.13 -m pip install ortools==9.14.6206

# Clean up
RUN python3.13 -m pip cache purge && \
    rm /tmp/freeze.txt

# Install online-judge-tools
RUN python3.13 -m pip install online-judge-tools==11.5.1

# nodejs, atcoder-cli
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

# AtCoder 参加用プログラミング言語
# C++
# RUN apt-get update && \
#     apt-get install -y --no-install-recommends libgmp3-dev unzip && \
#     cd /tmp && \
#     # ac library
#     mkdir -p /opt/ac-library && \
#     curl -OL https://github.com/atcoder/ac-library/releases/download/v1.5.1/ac-library.zip && \
#     unzip /tmp/ac-library.zip -d /opt/ac-library && \
#     rm /tmp/ac-library.zip && \
#     # boost
#     curl -OL https://boostorg.jfrog.io/artifactory/main/release/1.82.0/source/boost_1_82_0.tar.gz && \
#     tar xf boost_1_82_0.tar.gz && \
#     rm boost_1_82_0.tar.gz && \
#     cd boost_1_82_0 && \
#     ./bootstrap.sh --with-toolset=gcc --without-libraries=mpi,graph_parallel && \
#     ./b2 -j3 toolset=gcc variant=release link=static runtime-link=static cxxflags="-std=c++20" stage && \
#     ./b2 -j3 toolset=gcc variant=release link=static runtime-link=static cxxflags="-std=c++20" --prefix=/opt/boost/gcc install && \
#     cd /tmp && \
#     rm -rf /tmp/boost_1_82_0 && \
#     #Eigen
#     apt install -y libeigen3-dev=3.4.0-2ubuntu2 && \
#     apt-get clean && \
#     apt-get autoremove -y && \
#     rm -rf /var/lib/apt/lists/*

# Java
# OpenJDKのダウンロードとインストール
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

# AC Libraryのダウンロード
RUN wget -q https://github.com/ocha98/ac-library-java/releases/download/v2.0.0/ac_library23.jar && \
    mv ac_library23.jar ac_library.jar

# 実行スクリプトの作成
COPY java/java.sh /judge/java.sh
RUN chmod +x /judge/java.sh


# Ruby
# Install dependencies
RUN apt-get update && \
   DEBIAN_FRONTEND=noninteractive apt-get install -y \
   autoconf \
   bison \
   patch \
   build-essential \
# rustc removed - using official Rust installation instead
   libssl-dev \
   libyaml-dev \
   libreadline6-dev \
   zlib1g-dev \
   libgmp-dev \
   libncurses5-dev \
   libffi-dev \
   libgdbm6 \
   libgdbm-dev \
   libdb-dev \
   uuid-dev \
   libz3-dev \
   libgeos-dev \
   curl \
   wget \
   unzip \
   && rm -rf /var/lib/apt/lists/*

# Install libtorch (TOML compliant - without cxx11-abi)
WORKDIR /tmp
ARG AC_LIBTORCH_VERSION="2.8.0"
RUN wget -q -O libtorch.zip https://download.pytorch.org/libtorch/cpu/libtorch-shared-with-deps-${AC_LIBTORCH_VERSION}%2Bcpu.zip && \
   unzip -q libtorch.zip && \
   cd libtorch && \
   cp -dR include /usr/local/ && \
   cp -dR lib /usr/local/ && \
   echo /usr/local/lib/libtorch > /etc/ld.so.conf.d/libtorch.conf && \
   echo "libtorch $AC_LIBTORCH_VERSION" >> /root/library_version && \
   cd .. && \
   rm -rf libtorch*

# Install ruby-build and Ruby (TOML compliant - no GC patch)
RUN curl -s https://api.github.com/repos/rbenv/ruby-build/releases/latest | \
   grep -o 'https://[^"]*tarball[^"]*' | \
   xargs curl -o ruby-build.tarball -L && \
   tar -xf ruby-build.tarball && \
   PREFIX=/usr/local ./*ruby-build-*/install.sh && \
   ruby-build 3.4.5 /root/.rubies/ruby && \
   rm -rf *ruby-build* ruby-build.tarball

# Set Ruby PATH
ENV PATH=/root/.rubies/ruby/bin:$PATH

# Save default gems list
RUN gem list -q --no-versions > /tmp/default-gems

# Install Ruby gems
# 基本的なgemsのインストール（問題のないもの）
RUN gem install -N \
    ac-library-rb:1.2.0 \
    bit_utils:0.1.2 \
    bitarray:1.3.1 \
    fast_trie:0.5.1 \
    faster_prime:1.0.2 \
    ffi-geos:2.5.0 \
    immutable-ruby:0.2.0 \
    lightgbm:0.4.3 \
    numo-linalg:0.1.7 \
    numo-narray:0.9.2.1 \
    numo-openblas:0.5.1 \
    polars-df:0.21.1 \
    rbtree:0.4.6 \
    rgl:0.6.6 \
    rumale:1.0.0 \
    sorted_containers:1.1.0 \
    sorted_set:1.0.3 \
    z3:0.0.20230311

# or-toolsのインストール（aarch64用の特別な設定が必要）
RUN if [ "$(uname -m)" = "x86_64" ]; then \
    apt-get update && \
    apt-get install -y cmake && \
    git clone https://github.com/google/or-tools.git && \
    cd or-tools && \
    cmake -S . -B build -DBUILD_DEPS=ON && \
    cmake --build build --target install && \
    gem install or-tools -v 0.16.0; \
    gem install torch-rb -v 0.21.0 -- --with-torch-dir=/usr/local; \
    fi

# torch-rbのインストール（LibTorchのパスを指定）
#RUN gem install torch-rb -v 0.19.0 -- --with-torch-dir=/tmp/libtorch

#RUN . ${asdf_init} && \
#    apt-get update && \
#    apt-get install -y --no-install-recommends libyaml-dev && \
#    asdf plugin-add ruby https://github.com/asdf-vm/asdf-ruby.git && \
#    asdf install ruby 3.2.2 && \
#    asdf global ruby 3.2.2 && \
#    apt-get clean && \
#    apt-get autoremove -y && \
#    rm -rf /var/lib/apt/lists/*
#RUN . ${asdf_init} && \
#    gem install rbtree && \
#    gem install ac-library-rb && \
#    gem install faster_prime && \
#    gem install sorted_set && \
#    gem install numo-narray && \
#    gem install polars-df

# erlang
ARG AC_OTP_VERSION=28.0.2
RUN apt-get update && \
    apt-get install -y \
        libssl-dev \
        unixodbc-dev \
        libblas-dev \
        liblapack-dev \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /tmp
RUN wget -q -O erlang.tar.gz https://github.com/erlang/otp/releases/download/OTP-${AC_OTP_VERSION}/otp_src_${AC_OTP_VERSION}.tar.gz && \
    mkdir erlang && \
    tar -C erlang --strip-components=1 -xf erlang.tar.gz && \
    cd erlang && \
    ./configure --without-termcap && \
    make -j4 && \
    make install && \
    cd .. && \
    rm -rf erlang erlang.tar.gz

# elixir
# Install Elixir
# Elixir v1.18.4 precompiled for OTP 27 is binary compatible with OTP 28
RUN AC_OTP_MAJOR_VERSION=27 && \
    wget -q https://github.com/elixir-lang/elixir/releases/download/v1.18.4/elixir-otp-${AC_OTP_MAJOR_VERSION}.zip && \
    unzip elixir-otp-${AC_OTP_MAJOR_VERSION}.zip 'bin/*' 'lib/*' -d /usr/local && \
    rm elixir-otp-${AC_OTP_MAJOR_VERSION}.zip

# Create and setup Elixir project
WORKDIR /judge
RUN mix local.hex --force && \
    mix local.rebar --force && \
    mix new main && \
    cd main

COPY elixir/mix.exs /judge/main/
COPY elixir/config.exs /judge/main/config/
COPY elixir/main.ex /judge/main/lib/

WORKDIR /judge/main
ENV EXLA_BUILD=fast \
    EXLA_TARGET=cpu
# EXLA build parallelism is controlled via config/config.exs (config :exla, :make_args)
RUN MIX_ENV=prod mix deps.get && \
    MIX_ENV=prod mix release && \
    rm _build/prod/rel/main/bin/main

# PHP (from TOML install-script)
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y software-properties-common && \
    add-apt-repository -y ppa:ondrej/php && \
    apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
        php8.4-cli \
        php8.4-gmp \
        php8.4-bcmath \
        php8.4-sqlite3 && \
    rm -rf /var/lib/apt/lists/* && \
    echo "opcache.enable_cli = 1" >> /etc/php/8.4/cli/conf.d/10-opcache.ini && \
    echo "opcache.jit = tracing" >> /etc/php/8.4/cli/conf.d/10-opcache.ini && \
    echo "opcache.jit_buffer_size = 128M" >> /etc/php/8.4/cli/conf.d/10-opcache.ini

# Rust (from TOML install-script)
ARG RUST_VERSION=1.87.0
WORKDIR /tmp
RUN case "$(uname -m)" in \
        x86_64) \
            curl "https://static.rust-lang.org/dist/rust-${RUST_VERSION}-x86_64-unknown-linux-gnu.tar.gz" -fO && \
            tar xf "rust-${RUST_VERSION}-x86_64-unknown-linux-gnu.tar.gz" && \
            "./rust-${RUST_VERSION}-x86_64-unknown-linux-gnu/install.sh" --prefix=/usr/local \
            ;; \
        aarch64) \
            curl "https://static.rust-lang.org/dist/rust-${RUST_VERSION}-aarch64-unknown-linux-gnu.tar.gz" -fO && \
            tar xf "rust-${RUST_VERSION}-aarch64-unknown-linux-gnu.tar.gz" && \
            "./rust-${RUST_VERSION}-aarch64-unknown-linux-gnu/install.sh" --prefix=/usr/local \
            ;; \
        *) \
            echo "Unsupported architecture: $(uname -m)" \
            exit 1 \
            ;; \
    esac && \
    rm -rf rust-*

# Setup Rust project with competitive programming libraries (from TOML)
WORKDIR /judge
RUN mkdir -p .cargo src && \
    echo '[build]' > .cargo/config.toml && \
    echo 'rustflags = ["--cfg", "atcoder"]' >> .cargo/config.toml

# Copy Rust project files from TOML configuration
COPY rust/Cargo.toml rust/Cargo.lock /judge/
RUN echo 'fn main() {}' > src/main.rs && \
    cargo build --release && \
    rm target/release/main

## # AHC用のRustのinstall
#RUN curl https://sh.rustup.rs -sSf | sh -s -- -y
#ENV PATH $PATH:/home/root/.cargo/bin

# C++ ライブラリ追加 (TOML config.toml部分実装)
RUN apt-get update && \
    apt-get install -y \
        cmake \
        libeigen3-dev \
        libgmp-dev \
        libgmpxx4ldbl \
        && \
    rm -rf /var/lib/apt/lists/*

# range-v3 (軽量ヘッダーライブラリ)
ARG RANGEV3_VERSION=0.12.0
WORKDIR /tmp
RUN wget -q https://github.com/ericniebler/range-v3/archive/refs/tags/${RANGEV3_VERSION}.tar.gz -O range-v3.tar.gz && \
    tar -xzf range-v3.tar.gz && \
    cp -r range-v3-${RANGEV3_VERSION}/include/range /usr/local/include/ && \
    rm -rf range-v3*

# unordered_dense (軽量ハッシュマップライブラリ)
ARG UNORDERED_DENSE_VERSION=4.5.0
RUN wget -q https://github.com/martinus/unordered_dense/archive/refs/tags/v${UNORDERED_DENSE_VERSION}.tar.gz -O unordered_dense.tar.gz && \
    tar -xzf unordered_dense.tar.gz && \
    mkdir -p build_unordered_dense && \
    cd build_unordered_dense && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr/local ../unordered_dense-${UNORDERED_DENSE_VERSION} && \
    make install && \
    cd .. && \
    rm -rf unordered_dense* build_unordered_dense

# Boost (軽量版 - 一部ライブラリのみ)
ARG BOOST_VERSION=1.88.0
RUN apt-get update && \
    apt-get install -y \
        libboost-system-dev \
        libboost-filesystem-dev \
        libboost-program-options-dev \
        libboost-regex-dev \
        libboost-graph-dev \
        && \
    rm -rf /var/lib/apt/lists/*

# AC Library C++ (TOMLバージョン)
ARG AC_LIBRARY_VERSION=1.6
RUN wget -q https://github.com/atcoder/ac-library/archive/refs/tags/v${AC_LIBRARY_VERSION}.tar.gz -O ac-library.tar.gz && \
    tar -xzf ac-library.tar.gz && \
    cp -r ac-library-${AC_LIBRARY_VERSION}/atcoder /usr/local/include/ && \
    rm -rf ac-library* && \
    echo 'C++ libraries from TOML config: AC Library 1.6, Eigen3, range-v3 0.12.0, unordered_dense 4.5.0, Boost (subset)' > /usr/local/share/cpp-libraries.txt

# C, C++ のバージョン指定 (GCC 13に統一)
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 90 --slave /usr/bin/g++ g++ /usr/bin/g++-13

WORKDIR /root
