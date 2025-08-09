# Mesa Vulkan Wrapper CI Documentation

This document describes the Continuous Integration setup for the Mesa Vulkan wrapper, including the new Ubuntu CI workflow for testing nullDescriptors emulation.

## CI Workflows

### macOS CI (.github/workflows/macos.yml)
- **Purpose**: Tests Mesa builds on macOS
- **Features**: MoltenVK integration, brew dependencies
- **Matrix**: GLX options (dri, xlib)

### Ubuntu CI (.github/workflows/ubuntu.yml) 
- **Purpose**: Tests Mesa builds on Ubuntu with focus on Vulkan wrapper
- **Features**: Vulkan validation layers, nullDescriptors testing
- **Matrix**: Compiler (gcc, clang), Build type (debug, release)

## Ubuntu CI Details

### Dependencies Installed
```bash
# Build tools
build-essential pkg-config python3-pip meson ninja-build bison flex

# Mesa dependencies  
libdrm-dev libudev-dev

# X11/Wayland support
libx11-dev libxcb1-dev libxcb-dri2-0-dev libxcb-dri3-dev
libxcb-present-dev libxcb-sync-dev libxshmfence-dev
libxxf86vm-dev libxrandr-dev libxfixes-dev libxdamage-dev
libxext-dev libwayland-dev wayland-protocols libwayland-egl-backend-dev

# Vulkan support
vulkan-tools vulkan-validationlayers-dev libvulkan-dev
```

### Build Configuration
- **Minimal Build**: Only builds Vulkan wrapper driver
- **Disabled Features**: OpenGL, EGL, GBM, GLES, Android stub (to avoid compilation issues)
- **Vulkan Driver**: `wrapper` only
- **Test Mode**: Enabled for wrapper testing

### Test Pipeline

1. **Build Verification**
   - Confirms `libvulkan_wrapper.so` is built
   - Checks for required exported symbols
   
2. **Symbol Loading Test**
   - Verifies all essential Vulkan entry points are exported
   - Tests dynamic library loading
   
3. **nullDescriptors Logic Test**
   - Runs C-based unit test for descriptor substitution
   - Validates Python-based high-level emulation tests
   
4. **System Integration**
   - Runs `vulkaninfo` if available
   - Basic Vulkan system validation

### Artifacts
- Build logs (`build/meson-logs/`)
- Installation results (`install/`)
- Compiled wrapper library (`build/src/vulkan/wrapper/libvulkan_wrapper.so`)
- Retained for 5 days

## Testing Framework

### Core Tests

#### C Unit Tests (`tests/test_nulldescriptor_logic.c`)
- Tests descriptor substitution algorithm
- Validates null handle detection and replacement
- Mock Vulkan environment for isolated testing

#### Python Integration Tests (`tests/test_nulldescriptors.py`)  
- High-level emulation behavior validation
- Extension advertisement testing
- Feature reporting verification
- Device lifecycle testing

### Test Execution
```bash
# Run C unit test
gcc -o test_nulldescriptor_logic tests/test_nulldescriptor_logic.c
./test_nulldescriptor_logic

# Run Python tests
python3 tests/test_nulldescriptors.py
```

## Mesa Testing Guidelines

Following Mesa's testing best practices:

### Build Testing
- `ninja -C builddir` - Full build
- `ninja -C builddir test` - Run test suite (when available)

### Validation Testing  
- Unit tests for core functionality
- Integration tests for wrapper behavior
- Symbol export verification
- Basic Vulkan driver functionality

### CTS Integration (Future)
- Vulkan Conformance Test Suite integration
- Focus on robustness and nullDescriptors test cases
- Automated regression testing

## Adding New Tests

### C Unit Tests
1. Add test file to `tests/` directory
2. Include compilation in CI workflow
3. Ensure test follows Mesa conventions

### Python Tests  
1. Add test script to `tests/` directory
2. Update CI to execute Python test
3. Use standard Python test patterns

### Integration Tests
1. Mock Vulkan environment for isolated testing
2. Test wrapper-specific functionality
3. Validate emulation behavior

## Debugging CI Issues

### Common Build Failures
- **Missing Dependencies**: Check apt package installation
- **Meson Configuration**: Verify build options are correct
- **Symbol Errors**: Check wrapper library exports

### Test Failures
- **Logic Tests**: Review substitution algorithm
- **Symbol Tests**: Verify entry point exports
- **Integration Tests**: Check Vulkan environment setup

### Platform-Specific Issues
- **Ubuntu Version**: CI uses Ubuntu 24.04
- **Compiler Versions**: Tests both GCC and Clang
- **Vulkan Driver**: May need mock driver for testing

## Performance Monitoring

### Build Time Tracking
- Monitor CI build duration
- Track artifact sizes
- Memory usage during compilation

### Test Coverage
- Ensure nullDescriptors paths are tested
- Validate both emulated and native code paths
- Monitor test execution time

## Future Enhancements

### Extended Testing
- Add CTS robustness test subset
- DXVK compatibility testing
- Performance regression testing

### Additional Platforms
- Android emulator testing
- Windows cross-compilation testing
- ARM64 architecture support

### Automation Improvements
- Automatic performance benchmarking
- Code coverage reporting
- Nightly comprehensive testing