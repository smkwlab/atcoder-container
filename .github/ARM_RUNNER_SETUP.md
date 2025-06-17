# ARM Runner Setup Guide

## Overview

This document describes the setup and configuration for ARM64 builds using dedicated ARM runners to optimize build performance and resource usage.

## 🎯 Benefits of ARM Runners

### Performance
- **Native ARM64 builds** - No emulation overhead
- **Faster build times** - Direct hardware execution
- **Better resource utilization** - Optimized memory and CPU usage

### Cost Efficiency  
- **Reduced build time** - Lower CI costs
- **Parallel execution** - Independent AMD64/ARM64 builds
- **Selective usage** - Only for main branch and releases

## 🔧 Runner Configuration

### Requirements
- ARM64 hardware (Apple Silicon, AWS Graviton, etc.)
- Docker installed and configured
- GitHub Actions runner software
- Sufficient storage (20GB+ recommended for container builds)

### GitHub Runner Setup

1. **Register Runner**
   ```bash
   # Download and configure GitHub Actions runner
   mkdir actions-runner && cd actions-runner
   curl -o actions-runner-linux-arm64-2.311.0.tar.gz -L https://github.com/actions/runner/releases/download/v2.311.0/actions-runner-linux-arm64-2.311.0.tar.gz
   tar xzf ./actions-runner-linux-arm64-2.311.0.tar.gz
   
   # Configure with repository token
   ./config.sh --url https://github.com/smkwlab/atcoder-container --token YOUR_TOKEN --labels arm-runner
   ```

2. **Start Runner Service**
   ```bash
   # Install as service (Linux)
   sudo ./svc.sh install
   sudo ./svc.sh start
   
   # Or run interactively for testing
   ./run.sh
   ```

### Docker Configuration

```bash
# Ensure Docker is installed and user has permissions
sudo usermod -aG docker $USER
newgrp docker

# Test Docker functionality
docker run --rm hello-world
```

## 🚀 Workflow Integration

### Runner Selection Logic

The workflows automatically select appropriate runners:

```yaml
strategy:
  matrix:
    include:
      - platform: linux/amd64
        runner: ubuntu-latest
      - platform: linux/arm64
        runner: ${{ needs.determine-strategy.outputs.is_main_branch == 'true' && 'arm-runner' || 'ubuntu-latest' }}
```

### Fallback Strategy
- **Main branch**: Uses `arm-runner` for ARM64 builds
- **Feature branches**: Uses `ubuntu-latest` with QEMU emulation
- **PR testing**: AMD64 only for speed

## 📊 Build Strategy Matrix

| Event Type | Branch | AMD64 Runner | ARM64 Runner | Variants |
|------------|--------|--------------|--------------|----------|
| Push | main | ubuntu-latest | arm-runner | lite + full |
| Push | feature/* | ubuntu-latest | ubuntu-latest | lite only |
| PR | any | ubuntu-latest | - | lite only |
| Manual | any | ubuntu-latest | arm-runner | configurable |

## 🔒 Security Considerations

### Runner Security
- Use dedicated runners for trusted repositories only
- Regularly update runner software
- Monitor runner logs and activity
- Implement proper network isolation

### Access Control
- Limit runner access to necessary repositories
- Use GitHub's runner groups for organization
- Regular token rotation

## 🐛 Troubleshooting

### Common Issues

1. **Runner Offline**
   ```bash
   # Check runner status
   sudo systemctl status actions.runner.*
   
   # Restart if needed
   sudo systemctl restart actions.runner.*
   ```

2. **Docker Permission Issues**
   ```bash
   # Fix Docker permissions
   sudo usermod -aG docker $USER
   newgrp docker
   ```

3. **Storage Issues**
   ```bash
   # Clean up Docker resources
   docker system prune -af
   docker volume prune -f
   ```

### Monitoring

Monitor runner performance and availability:
- CPU and memory usage during builds
- Build completion times
- Network connectivity
- Storage utilization

## 📋 Maintenance Checklist

### Weekly
- [ ] Check runner status and logs
- [ ] Monitor resource usage
- [ ] Clean up Docker resources if needed

### Monthly  
- [ ] Update GitHub Actions runner
- [ ] Review security logs
- [ ] Check storage utilization

### Quarterly
- [ ] Review runner configuration
- [ ] Update documentation
- [ ] Performance optimization review

## 🔗 References

- [GitHub Actions Self-hosted Runners](https://docs.github.com/en/actions/hosting-your-own-runners)
- [Docker Build Push Action](https://github.com/docker/build-push-action)
- [ARM64 Docker Best Practices](https://docs.docker.com/build/building/multi-platform/)