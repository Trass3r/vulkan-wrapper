# Vulkan Wrapper Tests

This directory contains tests for the Vulkan wrapper's null descriptor emulation functionality.

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

## Usage

To run all tests:
```bash
# C unit tests
make test

# Python behavior tests  
python3 test_nulldescriptors.py

# Clean up
make clean
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