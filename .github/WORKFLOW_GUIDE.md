# GitHub Workflow Guide

## 🚀 Staged Build Strategy Overview

This repository implements a sophisticated staged build strategy to optimize CI/CD performance, resource usage, and development workflow efficiency.

## 📋 Workflow Types

### 1. Staged Container Build (`build-staged.yml`)
**Primary production workflow with intelligent build strategy**

#### Triggers
- **Push to main**: Full build (lite + full variants, both platforms)
- **Push to feature branches**: Lite build only (AMD64 only)
- **Manual dispatch**: Configurable variant selection
- **Commit message triggers**: `[build-all]` or `[build-full]`

#### Build Matrix
```
Stage 1: Strategy Determination
├── Analyze trigger event
├── Parse commit messages  
└── Set build flags

Stage 2: Lite Version Build (Priority)
├── AMD64: ubuntu-latest
└── ARM64: arm-runner (main) / ubuntu-latest (others)

Stage 3: Full Version Build (Conditional)
├── AMD64: ubuntu-latest
└── ARM64: arm-runner (main) / ubuntu-latest (others)

Stage 4: Summary & Notification
```

### 2. PR Test Build (`pr-test.yml`)
**Fast feedback for pull requests**

#### Features
- **Lite version only** - Quick validation
- **AMD64 platform only** - Maximum speed
- **Automatic testing** - Basic functionality verification
- **PR comments** - Build status and results
- **Concurrency control** - Cancel previous builds

### 3. Legacy Build (`build-latest.yml`)
**Original workflow - kept for compatibility**

## 🎛️ Build Control Methods

### Automatic Detection
```bash
# Lite version only (default for feature branches)
git commit -m "Fix login issue"

# Build all variants
git commit -m "Add new language support [build-all]"

# Build full version only
git commit -m "Update scientific libraries [build-full]"
```

### Manual Workflow Dispatch
1. Go to **Actions** tab
2. Select **Staged Container Build**
3. Click **Run workflow**
4. Choose options:
   - **Branch**: Target branch
   - **Build variant**: `lite-only` | `full-only` | `all-variants`
   - **Force build all**: Override auto-detection

### Environment Variables
```yaml
# Custom configuration in workflow files
env:
  FORCE_ARM_RUNNER: true    # Always use ARM runner
  SKIP_FULL_BUILD: true     # Lite only regardless of branch
  PLATFORMS_OVERRIDE: "linux/amd64,linux/arm64,linux/arm/v7"
```

## 🏗️ Container Variants

### Lite Version (`Dockerfile.lite`)
- **Size**: ~3.86GB
- **Focus**: Competitive programming
- **Languages**: All core languages with essential libraries
- **Use case**: AtCoder contests, basic development
- **Build time**: ~20-30 minutes

### Full Version (`Dockerfile`)  
- **Size**: ~8.12GB
- **Focus**: Complete development environment
- **Languages**: All languages + scientific computing libraries
- **Use case**: Research, data science, machine learning
- **Build time**: ~45-60 minutes

## 🖥️ Platform Support

### AMD64 (x86_64)
- **Runner**: `ubuntu-latest`
- **Availability**: All workflows
- **Primary target**: Development and testing

### ARM64 (aarch64)
- **Runner**: `arm-runner` (main branch) / `ubuntu-latest` (emulated)
- **Availability**: Main branch builds and manual dispatch
- **Primary target**: Apple Silicon, AWS Graviton, native ARM

## 📊 Performance Optimization

### Build Strategy Decision Tree
```
Event Type?
├── PR → Lite AMD64 only (fastest feedback)
├── Feature branch push → Lite both platforms
├── Main branch push → All variants, both platforms
└── Manual → User configurable

ARM Runner Available?
├── Yes (main branch) → Native ARM64 builds
└── No → QEMU emulation fallback

Cache Strategy?
├── Lite builds → scope=lite cache
├── Full builds → scope=full cache
└── Platform specific → per-platform cache
```

### Resource Efficiency
- **Parallel execution**: Independent platform builds
- **Smart caching**: Separate cache scopes for variants
- **Conditional builds**: Skip unnecessary builds
- **Early termination**: Fail fast on errors

## 🔧 Configuration Examples

### Development Workflow
```bash
# Feature development - quick testing
git checkout -b feature/new-algorithm
git commit -m "Implement new sorting algorithm"
git push
# → Triggers: Lite AMD64 build only

# Ready for full testing
git commit -m "Complete implementation [build-all]"
git push  
# → Triggers: All variants, both platforms
```

### Release Workflow
```bash
# Release preparation
git checkout main
git merge feature/new-algorithm
git commit -m "Release v2.1.0 with algorithm improvements"
git push
# → Triggers: Full production build (all variants, platforms)
```

### Emergency Builds
```bash
# Quick lite build test
gh workflow run "Staged Container Build" \
  --ref main \
  -f build_variant=lite-only

# Full rebuild with all variants
gh workflow run "Staged Container Build" \
  --ref main \
  -f build_variant=all-variants \
  -f force_build_all=true
```

## 🐛 Troubleshooting

### Common Issues

1. **ARM runner offline**
   - Fallback to emulated builds automatically
   - Check ARM_RUNNER_SETUP.md for maintenance

2. **Build timeout**
   - Full version builds may take 60+ minutes
   - Consider using lite version for testing

3. **Cache issues**
   - Clear cache: Actions → Caches → Delete relevant caches
   - Rebuild with fresh cache

### Monitoring

Check build health in:
- **Actions tab**: Recent workflow runs
- **Build summary**: Detailed reports in each run
- **PR comments**: Automated test results

## 📚 Best Practices

### For Contributors
1. **Use lite builds** for feature development
2. **Test locally first** before pushing
3. **Add `[build-all]`** for comprehensive testing
4. **Monitor PR feedback** for quick issue detection

### For Maintainers  
1. **Review ARM runner status** regularly
2. **Monitor build performance** and optimize as needed
3. **Update cache strategy** when adding new dependencies
4. **Document breaking changes** in workflow updates

## 🔗 Related Documentation

- [ARM Runner Setup](./ARM_RUNNER_SETUP.md) - ARM64 build configuration
- [Container Variants](../README.md) - Lite vs Full comparison
- [Development Guide](../CLAUDE.md) - General development guidance