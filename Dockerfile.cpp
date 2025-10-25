# AtCoder Container - C++ Professional Version
# Specialized container with C++ and comprehensive competitive programming libraries
# Stage 1: Build stage with all development tools
FROM ubuntu:24.04 AS builder

ARG DEBIAN_FRONTEND=noninteractive
ARG DEBCONF_NONINTERACTIVE_SEEN=true
ENV TZ=Asia/Tokyo

# Install all build dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
            curl \
            git \
            ca-certificates \
            locales \
            wget \
            unzip \
            xz-utils \
            # Build tools for C++ compilation
            build-essential \
            cmake \
            lld \
            ninja-build \
            pigz \
            pbzip2 \
            ccache \
            # Library build dependencies
            libgmp3-dev \
            libbz2-dev \
            zlib1g-dev \
            && \
    localedef -f UTF-8 -i ja_JP ja_JP.UTF-8 && \
    rm -rf /var/lib/apt/lists/*

ENV LANG="ja_JP.UTF-8" \
    LANGUAGE="ja_JP:ja" \
    LC_ALL="ja_JP.UTF-8" \
    ATCODER=1

# Set install directories
ENV AC_INSTALL_DIR=/opt/atcoder/gcc \
    AC_TEMP_DIR=/tmp/atcoder/gcc

RUN mkdir -p "$AC_INSTALL_DIR/include" "$AC_INSTALL_DIR/lib" "$AC_TEMP_DIR" && \
    mkdir -p /etc/atcoder && \
    echo "$AC_INSTALL_DIR" > /etc/atcoder/install_dir.txt

# Build GCC 15.2.0 from source
ARG GCC_VERSION=15.2.0
WORKDIR $AC_TEMP_DIR
RUN wget -q "http://ftp.tsukuba.wide.ad.jp/software/gcc/releases/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.gz" -O gcc.tar.gz && \
    mkdir gcc && \
    tar -I pigz -xf gcc.tar.gz -C gcc --strip-components 1 && \
    cd gcc && \
    ./contrib/download_prerequisites && \
    mkdir build && cd build && \
    ../configure \
        CC="ccache gcc" \
        CXX="ccache g++" \
        --prefix="$AC_INSTALL_DIR" \
        --enable-languages=c++ \
        --disable-bootstrap \
        --disable-multilib \
        --disable-libsanitizer \
        --disable-checking \
        --disable-nls \
        --disable-gcov \
        --disable-libada \
        --disable-libgm2 && \
    make -j$(( $(nproc) + 2 )) >/dev/null && \
    make install && \
    cd ../.. && \
    rm -rf gcc gcc.tar.gz

# Set compiler paths
ENV PATH="$AC_INSTALL_DIR/bin:$PATH" \
    CC="$AC_INSTALL_DIR/bin/gcc" \
    CXX="$AC_INSTALL_DIR/bin/g++" \
    LD_LIBRARY_PATH="$AC_INSTALL_DIR/lib64:$LD_LIBRARY_PATH"

# Build Abseil 20250512.1
ARG ABSEIL_VERSION=20250512.1
WORKDIR $AC_TEMP_DIR
RUN wget -q "https://github.com/abseil/abseil-cpp/releases/download/$ABSEIL_VERSION/abseil-cpp-$ABSEIL_VERSION.tar.gz" -O abseil.tar.gz && \
    mkdir abseil && \
    tar -I pigz -xf abseil.tar.gz -C abseil --strip-components 1 && \
    cd abseil && \
    mkdir build && cd build && \
    cmake -G "Ninja" \
        -DCMAKE_C_COMPILER="$CC" \
        -DCMAKE_CXX_COMPILER="$CXX" \
        -DCMAKE_C_COMPILER_LAUNCHER=ccache \
        -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
        -DLINK_FLAGS:STRING="-fuse-ld=lld" \
        -DCFLAGS:STRING="-w" \
        -DCXXFLAGS:STRING="-w" \
        -DCMAKE_INSTALL_MESSAGE:STRING=NEVER \
        -DABSL_ENABLE_INSTALL:BOOL=ON \
        -DABSL_PROPAGATE_CXX_STD:BOOL=ON \
        -DABSL_USE_SYSTEM_INCLUDES:BOOL=ON \
        -DCMAKE_INSTALL_PREFIX:PATH="$AC_INSTALL_DIR" \
        -DCMAKE_CXX_FLAGS:STRING="-fPIC -std=gnu++23 -w -L$AC_INSTALL_DIR/lib64 -Wl,-R$AC_INSTALL_DIR/lib64" \
        .. && \
    cmake --build . --target install && \
    cd ../.. && \
    rm -rf abseil abseil.tar.gz

# Install AC Library C++ 1.6
ARG AC_LIBRARY_VERSION=1.6
WORKDIR $AC_TEMP_DIR
RUN wget -q "https://github.com/atcoder/ac-library/archive/refs/tags/v$AC_LIBRARY_VERSION.tar.gz" -O ac-library.tar.gz && \
    mkdir ac-library && \
    tar -I pigz -xf ac-library.tar.gz -C ac-library --strip-components 1 && \
    cp -rf ac-library/atcoder "$AC_INSTALL_DIR/include/" && \
    rm -rf ac-library ac-library.tar.gz

# Build Boost 1.88.0
ARG BOOST_VERSION=1.88.0
WORKDIR $AC_TEMP_DIR
RUN wget -q "https://archives.boost.io/release/$BOOST_VERSION/source/boost_1_88_0.tar.bz2" -O boost.tar.bz2 && \
    mkdir boost && \
    tar -I pbzip2 -xf boost.tar.bz2 -C boost --strip-components 1 && \
    cd boost && \
    echo "using gcc : : ccache $CXX ;" > user-config.jam && \
    sed -i \
        -e 's/test_compiler g++\$TOOLSET_SUFFIX/test_compiler "ccache g++\$TOOLSET_SUFFIX"/g' \
        ./tools/build/src/engine/build.sh && \
    ./bootstrap.sh \
        --with-toolset=gcc \
        --without-libraries=mpi,graph_parallel,python \
        --prefix="$AC_INSTALL_DIR" && \
    ./b2 \
        toolset=gcc \
        link=static \
        threading=single \
        variant=release \
        cflags="-w" \
        cxxflags="-w -std=gnu++23 -L$AC_INSTALL_DIR/lib64 -Wl,-R$AC_INSTALL_DIR/lib64" \
        --user-config=./user-config.jam \
        -j$(( $(nproc) + 2 )) -d0 \
        install && \
    cd .. && \
    rm -rf boost boost.tar.bz2

# Install Eigen3 3.4.0
ARG EIGEN_VERSION=3.4.0-4
WORKDIR $AC_TEMP_DIR
RUN apt-get update && \
    apt-get install -y --no-install-recommends "libeigen3-dev=$EIGEN_VERSION" && \
    mkdir -p "$AC_INSTALL_DIR/cmake" && \
    cp -Trf /usr/include/eigen3 "$AC_INSTALL_DIR/include" && \
    cp -f \
        /usr/share/eigen3/cmake/Eigen3Targets.cmake \
        /usr/share/eigen3/cmake/Eigen3Config.cmake \
        "$AC_INSTALL_DIR/cmake" && \
    sed -i \
        -e "s|/usr/include/eigen3|$AC_INSTALL_DIR/include|g" \
        "$AC_INSTALL_DIR/cmake/Eigen3Targets.cmake" && \
    apt-get remove -y libeigen3-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Build immer 0.8.1 (header-only)
ARG IMMER_VERSION=0.8.1
WORKDIR $AC_TEMP_DIR
RUN wget -q "https://github.com/arximboldi/immer/archive/refs/tags/v$IMMER_VERSION.tar.gz" -O immer.tar.gz && \
    mkdir immer && \
    tar -I pigz -xf immer.tar.gz -C immer --strip-components 1 && \
    cp -Trf immer/immer "$AC_INSTALL_DIR/include/immer" && \
    rm -rf immer immer.tar.gz

# Download and install LibTorch 2.8.0
ARG LIBTORCH_VERSION=2.8.0
WORKDIR $AC_TEMP_DIR
RUN wget "https://download.pytorch.org/libtorch/cpu/libtorch-shared-with-deps-$LIBTORCH_VERSION%2Bcpu.zip" -O libtorch.zip && \
    unzip -o libtorch.zip -d . && \
    rm -f libtorch/lib/libprotobuf.a libtorch/lib/libprotobuf-lite.a libtorch/lib/libprotoc.a && \
    cp -Trf libtorch/include "$AC_INSTALL_DIR/include" && \
    cp -Trf libtorch/lib "$AC_INSTALL_DIR/lib" && \
    rm -rf libtorch libtorch.zip

# Build LightGBM 4.6.0
ARG LIGHTGBM_VERSION=4.6.0
WORKDIR $AC_TEMP_DIR
RUN wget -q "https://github.com/microsoft/LightGBM/releases/download/v$LIGHTGBM_VERSION/lightgbm-$LIGHTGBM_VERSION.tar.gz" -O light-gbm.tar.gz && \
    mkdir light-gbm && \
    tar -I pigz -xf light-gbm.tar.gz -C light-gbm --strip-components 1 && \
    cd light-gbm && \
    rm -rf lightgbm external_libs/eigen && \
    mkdir build && cd build && \
    cmake -G "Ninja" \
        -DCMAKE_C_COMPILER="$CC" \
        -DCMAKE_CXX_COMPILER="$CXX" \
        -DCMAKE_C_COMPILER_LAUNCHER=ccache \
        -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
        -DLINK_FLAGS:STRING="-fuse-ld=lld" \
        -DCFLAGS:STRING="-w" \
        -DCXXFLAGS:STRING="-w" \
        -DCMAKE_INSTALL_MESSAGE:STRING=NEVER \
        -DBUILD_CLI:BOOL=OFF \
        -DBUILD_STATIC_LIB=ON \
        -DUSE_HOMEBREW_FALLBACK=OFF \
        -DCMAKE_INSTALL_PREFIX:PATH="$AC_INSTALL_DIR" \
        -DCMAKE_CXX_FLAGS:STRING="-std=gnu++23 -w -L$AC_INSTALL_DIR/lib64 -Wl,-R$AC_INSTALL_DIR/lib64 -I$AC_INSTALL_DIR/include -fopenmp" \
        .. && \
    cmake --build . --target install && \
    cd ../.. && \
    rm -rf light-gbm light-gbm.tar.gz

# Build OR-Tools 9.14
ARG OR_TOOLS_VERSION=9.14
WORKDIR $AC_TEMP_DIR
RUN wget -q "https://github.com/google/or-tools/archive/refs/tags/v$OR_TOOLS_VERSION.tar.gz" -O or-tools.tar.gz && \
    mkdir or-tools && \
    tar -I pigz -xf or-tools.tar.gz -C or-tools --strip-components 1 && \
    cd or-tools && \
    mkdir build && cd build && \
    cmake -G "Ninja" \
        -DCMAKE_C_COMPILER="$CC" \
        -DCMAKE_CXX_COMPILER="$CXX" \
        -DCMAKE_C_COMPILER_LAUNCHER=ccache \
        -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
        -DLINK_FLAGS:STRING="-fuse-ld=lld" \
        -DCFLAGS:STRING="-w" \
        -DCXXFLAGS:STRING="-w" \
        -DCMAKE_INSTALL_MESSAGE:STRING=NEVER \
        -DBUILD_CXX:BOOL=ON \
        -DBUILD_BZip2:BOOL=ON -DBUILD_ZLIB:BOOL=ON -DBUILD_Protobuf:BOOL=ON -DBUILD_re2:BOOL=ON \
        -DUSE_COINOR:BOOL=ON -DBUILD_CoinUtils:BOOL=ON -DBUILD_Osi:BOOL=ON -DBUILD_Clp:BOOL=ON -DBUILD_Cgl:BOOL=ON -DBUILD_Cbc:BOOL=ON \
        -DUSE_GLPK:BOOL=ON -DBUILD_GLPK:BOOL=ON \
        -DUSE_HIGHS:BOOL=ON -DBUILD_HIGHS:BOOL=ON \
        -DUSE_SCIP:BOOL=ON -DBUILD_SCIP:BOOL=ON -DBUILD_soplex:BOOL=ON -DBUILD_Boost:BOOL=ON \
        -DBUILD_SAMPLES:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF \
        -DBUILD_TESTING:BOOL=OFF \
        -DCMAKE_PREFIX_PATH:PATH="$AC_INSTALL_DIR" \
        -DCMAKE_INSTALL_PREFIX:PATH="$AC_INSTALL_DIR" \
        -DBUILD_SHARED_LIBS:BOOL=OFF \
        -DCMAKE_INSTALL_RPATH_USE_LINK_PATH:BOOL=ON \
        -DCMAKE_CXX_FLAGS:STRING="-std=gnu++23 -w -L$AC_INSTALL_DIR/lib64 -Wl,-R$AC_INSTALL_DIR/lib64" \
        .. && \
    cmake --build . --config Release --target install && \
    cd ../.. && \
    rm -rf or-tools or-tools.tar.gz

# Build range-v3 0.12.0 (header-only)
ARG RANGE_V3_VERSION=0.12.0
WORKDIR $AC_TEMP_DIR
RUN wget -q "https://github.com/ericniebler/range-v3/archive/refs/tags/$RANGE_V3_VERSION.tar.gz" -O range-v3.tar.gz && \
    mkdir range-v3 && \
    tar -I pigz -xf range-v3.tar.gz -C range-v3 --strip-components 1 && \
    cp -Trf range-v3/include "$AC_INSTALL_DIR/include" && \
    rm -rf range-v3 range-v3.tar.gz

# Build unordered_dense 4.5.0
ARG UNORDERED_DENSE_VERSION=4.5.0
WORKDIR $AC_TEMP_DIR
RUN wget "https://github.com/martinus/unordered_dense/archive/refs/tags/v$UNORDERED_DENSE_VERSION.tar.gz" -O unordered_dense.tar.gz && \
    mkdir unordered_dense && \
    tar -I pigz -xf unordered_dense.tar.gz -C unordered_dense --strip-components 1 && \
    cd unordered_dense && \
    mkdir build && cd build && \
    cmake -G "Ninja" \
        -DCMAKE_C_COMPILER="$CC" \
        -DCMAKE_CXX_COMPILER="$CXX" \
        -DCMAKE_C_COMPILER_LAUNCHER=ccache \
        -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
        -DLINK_FLAGS:STRING="-fuse-ld=lld" \
        -DCFLAGS:STRING="-w" \
        -DCXXFLAGS:STRING="-w" \
        -DCMAKE_INSTALL_MESSAGE:STRING=NEVER \
        -DCMAKE_INSTALL_PREFIX:PATH="$AC_INSTALL_DIR" \
        .. && \
    cmake --build . --target install && \
    cd ../.. && \
    rm -rf unordered_dense unordered_dense.tar.gz

# Build Z3 4.15.2
ARG Z3_VERSION=4.15.2
WORKDIR $AC_TEMP_DIR
RUN wget -q "https://github.com/Z3Prover/z3/archive/refs/tags/z3-$Z3_VERSION.tar.gz" -O z3.tar.gz && \
    mkdir z3 && \
    tar -I pigz -xf z3.tar.gz -C z3 --strip-components 1 && \
    cd z3 && \
    mkdir build && cd build && \
    cmake -G "Ninja" \
        -DCMAKE_C_COMPILER="$CC" \
        -DCMAKE_CXX_COMPILER="$CXX" \
        -DCMAKE_C_COMPILER_LAUNCHER=ccache \
        -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
        -DLINK_FLAGS:STRING="-fuse-ld=lld" \
        -DCFLAGS:STRING="-w" \
        -DCXXFLAGS:STRING="-w" \
        -DCMAKE_INSTALL_MESSAGE:STRING=NEVER \
        -DCMAKE_BUILD_TYPE:STRING=Release \
        -DCMAKE_INSTALL_PREFIX:PATH="$AC_INSTALL_DIR" \
        -DCMAKE_CXX_FLAGS:STRING="-std=gnu++23 -w -L$AC_INSTALL_DIR/lib64 -Wl,-R$AC_INSTALL_DIR/lib64" \
        .. && \
    cmake --build . --target install && \
    cd ../.. && \
    rm -rf z3 z3.tar.gz

# Stage 2: Runtime stage with minimal dependencies
FROM ubuntu:24.04 AS runtime

ARG DEBIAN_FRONTEND=noninteractive
ARG DEBCONF_NONINTERACTIVE_SEEN=true
ENV TZ=Asia/Tokyo

# Install runtime dependencies only
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
            curl \
            git \
            ca-certificates \
            locales \
            time \
            tzdata \
            wget \
            # Runtime libraries
            libgmp10 \
            libbz2-1.0 \
            zlib1g \
            libgomp1 \
            && \
    localedef -f UTF-8 -i ja_JP ja_JP.UTF-8 && \
    apt-get clean && \
    apt-get autoremove -y && \
    rm -rf /var/lib/apt/lists/*

ENV LANG="ja_JP.UTF-8" \
    LANGUAGE="ja_JP:ja" \
    LC_ALL="ja_JP.UTF-8" \
    ATCODER=1

# Copy built compiler and libraries from builder stage
COPY --from=builder /opt/atcoder /opt/atcoder
COPY --from=builder /etc/atcoder /etc/atcoder

# Set up paths
ENV PATH="/opt/atcoder/gcc/bin:$PATH" \
    LD_LIBRARY_PATH="/opt/atcoder/gcc/lib64:/opt/atcoder/gcc/lib:$LD_LIBRARY_PATH" \
    AC_INSTALL_DIR=/opt/atcoder/gcc

# Create compile script
RUN mkdir -p /usr/local/bin && \
    cat > /usr/local/bin/atcoder-compile <<'COMPILE_SCRIPT' && \
#!/bin/bash
set -eu

INSTALL_DIR="$(cat /etc/atcoder/install_dir.txt)"
USER_BUILD_FLAGS=(
"-DATCODER"
"-DNOMINMAX"
"-DONLINE_JUDGE"
"-DOR_PROTO_DLL="
"-DPROTOBUF_USE_DLLS"
"-DUSE_BOP"
"-DUSE_CBC"
"-DUSE_CLP"
"-DUSE_GLOP"
"-DUSE_LP_PARSER"
"-DUSE_MATH_OPT"
"-DUSE_PDLP"
"-DUSE_SCIP"
"-I${INSTALL_DIR}/include"
"-I${INSTALL_DIR}/include/torch/csrc/api/include"
"-O2"
"-Wall"
"-Wextra"
"-fconstexpr-depth=1024"
"-fconstexpr-loop-limit=524288"
"-fconstexpr-ops-limit=2097152"
"-flto=auto"
"-fmodules"
"-ftrivial-auto-var-init=zero"
"-march=native"
"-pthread"
"-std=gnu++23"
"-Wl,--as-needed"
"-L${INSTALL_DIR}/lib64"
"-Wl,-R${INSTALL_DIR}/lib64"
"-L${INSTALL_DIR}/lib"
"-Wl,-R${INSTALL_DIR}/lib"
"-fopenmp"
"-lstdc++exp"
"-labsl_cordz_sample_token"
"-labsl_failure_signal_handler"
"-labsl_flags_parse"
"-labsl_flags_usage"
"-labsl_flags_usage_internal"
"-labsl_log_flags"
"-labsl_periodic_sampler"
"-labsl_poison"
"-labsl_random_internal_distribution_test_util"
"-labsl_scoped_set_env"
"-lboost_atomic"
"-lboost_charconv"
"-lboost_chrono"
"-lboost_container"
"-lboost_context"
"-lboost_contract"
"-lboost_coroutine"
"-lboost_date_time"
"-lboost_exception"
"-lboost_fiber"
"-lboost_filesystem"
"-lboost_graph"
"-lboost_iostreams"
"-lboost_json"
"-lboost_locale"
"-lboost_log"
"-lboost_log_setup"
"-lboost_math_c99"
"-lboost_math_c99f"
"-lboost_math_c99l"
"-lboost_math_tr1"
"-lboost_math_tr1f"
"-lboost_math_tr1l"
"-lboost_nowide"
"-lboost_prg_exec_monitor"
"-lboost_process"
"-lboost_program_options"
"-lboost_random"
"-lboost_regex"
"-lboost_serialization"
"-lboost_stacktrace_from_exception"
"-lboost_system"
"-lboost_test_exec_monitor"
"-lboost_thread"
"-lboost_timer"
"-lboost_type_erasure"
"-lboost_unit_test_framework"
"-lboost_url"
"-lboost_wave"
"-lboost_wserialization"
"-lgmpxx"
"-lgmp"
"-lortools"
"-lCbc"
"-lCbcSolver"
"-lCgl"
"-lClp"
"-lClpSolver"
"-lCoinUtils"
"-lGLPK"
"-lOsi"
"-lOsiCbc"
"-lOsiClp"
"-lhighs"
"-lscip"
"-lz"
"-lbz2"
"-lprotobuf"
"-labsl_die_if_null"
"-labsl_log_initialize"
"-labsl_random_distributions"
"-labsl_random_seed_sequences"
"-labsl_random_internal_entropy_pool"
"-labsl_random_internal_randen"
"-labsl_random_internal_randen_hwaes"
"-labsl_random_internal_randen_hwaes_impl"
"-labsl_random_internal_randen_slow"
"-labsl_random_internal_platform"
"-labsl_random_internal_seed_material"
"-labsl_random_seed_gen_exception"
"-labsl_statusor"
"-labsl_status"
"-lutf8_validity"
"-lutf8_range"
"-pthread"
"-lre2"
"-labsl_log_internal_check_op"
"-labsl_leak_check"
"-labsl_log_internal_conditions"
"-labsl_log_internal_message"
"-labsl_examine_stack"
"-labsl_log_internal_format"
"-labsl_log_internal_nullguard"
"-labsl_log_internal_structured_proto"
"-labsl_log_internal_proto"
"-labsl_log_internal_log_sink_set"
"-labsl_log_internal_globals"
"-labsl_log_globals"
"-labsl_log_sink"
"-labsl_strerror"
"-labsl_vlog_config_internal"
"-labsl_log_internal_fnmatch"
"-labsl_flags_internal"
"-labsl_flags_marshalling"
"-labsl_flags_reflection"
"-labsl_flags_private_handle_accessor"
"-labsl_flags_commandlineflag"
"-labsl_flags_commandlineflag_internal"
"-labsl_flags_config"
"-labsl_flags_program_name"
"-labsl_raw_hash_set"
"-labsl_cord"
"-labsl_cordz_info"
"-labsl_cord_internal"
"-labsl_cordz_functions"
"-labsl_cordz_handle"
"-labsl_crc_cord_state"
"-labsl_crc32c"
"-labsl_crc_internal"
"-labsl_crc_cpu_detect"
"-labsl_hashtablez_sampler"
"-labsl_exponential_biased"
"-labsl_hash"
"-labsl_city"
"-labsl_low_level_hash"
"-labsl_str_format_internal"
"-labsl_synchronization"
"-labsl_graphcycles_internal"
"-labsl_kernel_timeout_internal"
"-labsl_stacktrace"
"-labsl_symbolize"
"-labsl_debugging_internal"
"-labsl_demangle_internal"
"-labsl_demangle_rust"
"-labsl_decode_rust_punycode"
"-labsl_utf8_for_code_point"
"-labsl_malloc_internal"
"-labsl_time"
"-labsl_civil_time"
"-labsl_strings"
"-labsl_strings_internal"
"-labsl_string_view"
"-labsl_int128"
"-labsl_throw_delegate"
"-labsl_time_zone"
"-labsl_tracing_internal"
"-labsl_base"
"-lrt"
"-labsl_raw_logging_internal"
"-labsl_log_severity"
"-labsl_spinlock_wait"
"-lz3"
"-l_lightgbm"
"-ltorch"
"-ltorch_cpu"
"-lc10"
)

g++ "${1:-Main.cpp}" -o a.out "${USER_BUILD_FLAGS[@]}"
COMPILE_SCRIPT
RUN chmod +x /usr/local/bin/atcoder-compile

# Create info file
RUN echo 'AtCoder C++ Professional: GCC 15.2.0, Boost 1.88.0, Abseil 20250512.1, LibTorch 2.8.0, OR-Tools 9.14, Eigen 3.4.0, Z3 4.15.2, AC Library 1.6, and more' > /usr/local/share/container-info.txt

# Set final working directory
WORKDIR /root
