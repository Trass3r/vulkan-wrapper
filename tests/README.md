# Vulkan Wrapper Tests

This directory contains tests for the Vulkan wrapper's null descriptor emulation and BC texture compression emulation functionality.

## Test Files

### `test_nulldescriptor_logic.c`
Unit tests for the core null descriptor substitution logic. Tests the basic functionality of replacing null handles with dummy resources.

**To build and run:**
```bash
make test
```

**Tests covered:**
- Null buffer descriptor substitution
- Null image descriptor substitution  
- Behavior when emulation is disabled

### `test_nulldescriptors.py`
High-level behavior tests for null descriptor emulation. Tests the overall wrapper behavior and integration scenarios.

**To run:**
```bash
python3 test_nulldescriptors.py
```

**Tests covered:**
- Native support detection logic
- Descriptor write interception
- Template update handling
- Descriptor buffer stub functionality
- Android compatibility features

### `test_bc_simple.c`
Basic tests for BC texture compression emulation logic without requiring full Vulkan setup.

**To build and run:**
```bash
make test  # Included in basic test suite
```

**Tests covered:**
- BC format constant validation
- Block size calculation logic
- Environment variable setup
- Basic decompression data validation

### `test_bc_emulation.c`
Comprehensive Vulkan integration tests for BC texture compression emulation (requires Vulkan driver).

**To build:**
```bash
make test_bc_emulation
```

**Tests covered:**
- BC format properties queries
- BC image creation and view creation
- Memory requirements for BC images
- Integration with Vulkan drivers

## Usage

To run basic tests:
```bash
# C unit tests
make test

# Python behavior tests  
python3 test_nulldescriptors.py

# Clean up
make clean
```

To run full Vulkan BC tests (requires Vulkan-capable system):
```bash
# Set environment to enable BC emulation
export WRAPPER_DEBUG=2

# Build and run
make test_bc_emulation
./test_bc_emulation
```

## CI Integration

These tests are automatically run in the CI pipeline when:
- Changes are made to the nulldescriptors branch
- Merge requests are created

The tests are executed in the `wrapper-test-nulldescriptors` CI job defined in `.gitlab-ci/wrapper-ci.yml`.

## Adding New Tests

When adding new null descriptor functionality:

1. Add unit tests in `test_nulldescriptor_logic.c` for core logic
2. Add behavior tests in `test_nulldescriptors.py` for integration scenarios
3. Update this README with new test descriptions
4. Ensure tests run successfully in CI

When adding new BC texture compression functionality:

1. Add basic logic tests in `test_bc_simple.c` for non-Vulkan functionality
2. Add integration tests in `test_bc_emulation.c` for full Vulkan scenarios
3. Test with the environment variable `WRAPPER_DEBUG=2` to enable BC emulation
4. Verify tests work both with and without native BC texture support