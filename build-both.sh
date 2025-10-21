#!/bin/bash

# AtCoder Container Builder - Full and Lite versions

set -e

echo "=========================================="
echo "AtCoder Container Builder"
echo "=========================================="

build_full() {
    echo "Building FULL version (atcoder-full:2025)..."
    echo "- Includes: PyTorch, LibTorch, torch-rb, scikit-learn, etc."
    echo "- Expected size: ~8GB"
    echo "- Build time: 1-2 hours"
    echo ""
    
    docker build -f Dockerfile -t atcoder-full:2025 . 2>&1 | tee build-full.log
    
    if [ $? -eq 0 ]; then
        echo "✅ Full version build completed successfully!"
        docker images atcoder-full:2025 --format "table {{.Repository}}:{{.Tag}}\t{{.Size}}"
    else
        echo "❌ Full version build failed. Check build-full.log for details."
        return 1
    fi
}

build_lite() {
    echo "Building LITE version (atcoder-lite:2025)..."
    echo "- Includes: Essential competitive programming libraries only"
    echo "- Excludes: PyTorch, LibTorch, torch-rb, heavy ML libraries"
    echo "- Expected size: ~3-4GB"
    echo "- Build time: 30-60 minutes"
    echo ""
    
    docker build -f Dockerfile.lite -t atcoder-lite:2025 . 2>&1 | tee build-lite.log
    
    if [ $? -eq 0 ]; then
        echo "✅ Lite version build completed successfully!"
        docker images atcoder-lite:2025 --format "table {{.Repository}}:{{.Tag}}\t{{.Size}}"
    else
        echo "❌ Lite version build failed. Check build-lite.log for details."
        return 1
    fi
}

verify_image() {
    local image=$1
    echo "Verifying $image..."
    
    docker run --rm $image bash -c "
        echo 'Language versions:'
        python3.13 --version
        node --version
        ruby --version
        java --version | head -1
        erl -eval 'erlang:display(erlang:system_info(otp_release)), halt().' -noshell
        elixir --version | head -1
        
        echo ''
        echo 'Essential tools:'
        oj --version
        acc -h | head -5
    " || echo "⚠️  Verification failed for $image"
}

case "${1:-both}" in
    "full")
        build_full
        verify_image "atcoder-full:2025"
        ;;
    "lite")
        build_lite
        verify_image "atcoder-lite:2025"
        ;;
    "both")
        echo "Building both versions..."
        echo ""
        build_lite
        echo ""
        echo "=========================================="
        echo ""
        build_full
        echo ""
        echo "=========================================="
        echo "Build Summary:"
        docker images | grep "atcoder.*2025"
        echo ""
        echo "Verification:"
        verify_image "atcoder-lite:2025"
        echo ""
        verify_image "atcoder-full:2025"
        ;;
    *)
        echo "Usage: $0 [full|lite|both]"
        echo ""
        echo "Options:"
        echo "  full  - Build full version only (atcoder-full:2025)"
        echo "  lite  - Build lite version only (atcoder-lite:2025)"
        echo "  both  - Build both versions (default)"
        exit 1
        ;;
esac

echo ""
echo "Done! 🎉"