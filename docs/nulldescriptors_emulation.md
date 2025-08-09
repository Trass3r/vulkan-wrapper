# nullDescriptors Emulation in Mesa Vulkan Wrapper

This document describes the implementation of nullDescriptors emulation from VK_KHR_robustness2 in Mesa's Vulkan wrapper for Android bionic.

## Overview

The nullDescriptors feature allows applications to use VK_NULL_HANDLE as descriptors in descriptor sets, which is particularly useful for applications like DXVK that may need to bind null resources. When the native Vulkan driver doesn't support this feature, the wrapper provides emulation.

## Implementation Details

### Detection Logic

The wrapper detects nullDescriptors support during physical device initialization:

1. **Native Support Check**: Queries if VK_KHR_robustness2 extension exists
2. **Feature Check**: If extension exists, checks if `nullDescriptor` feature is supported
3. **Emulation Decision**: If either extension or feature is missing, enables emulation

Key files:
- `wrapper_physical_device.c`: `wrapper_check_robustness2_emulation()`

### Dummy Resource System

When emulation is enabled, the wrapper creates dummy resources during device creation:

- **Dummy Buffer**: 1-byte buffer supporting uniform/storage/texel buffer usage
- **Dummy Images**: 1x1 pixel images for 1D, 2D, and 3D image types in R8G8B8A8_UNORM format
- **Dummy Image Views**: Corresponding image views for each image type
- **Dummy Sampler**: Basic sampler with nearest filtering and clamp-to-edge addressing

Key files:
- `wrapper_device.c`: `wrapper_create_dummy_resources()`
- `wrapper_device.c`: `wrapper_destroy_dummy_resources()`

### Descriptor Update Interception

The wrapper intercepts `vkUpdateDescriptorSets` and `vkUpdateDescriptorSetWithTemplate`:

1. **Null Detection**: Scans descriptor writes for VK_NULL_HANDLE resources
2. **Substitution**: Replaces null handles with appropriate dummy resources:
   - Null buffers → dummy buffer
   - Null image views → dummy image view (defaults to 2D)
   - Null samplers → dummy sampler
3. **Pass-through**: Forwards modified descriptor writes to the native driver

Key files:
- `wrapper_device.c`: `substitute_null_descriptors()`
- `wrapper_device.c`: `wrapper_UpdateDescriptorSets()`

### Extension Advertisement

When emulation is active:
- `VK_KHR_robustness2` is advertised in device extension enumeration
- `nullDescriptor` feature is reported as `VK_TRUE` in feature queries

Key files:
- `wrapper_physical_device.c`: Extension setup and feature reporting

## Data Structures

### Physical Device Extensions
```c
struct wrapper_physical_device {
    // ... existing fields ...
    bool robustness2_emulated;        // True if we're emulating the extension
    bool null_descriptors_emulated;   // True if we're emulating nullDescriptor
};
```

### Device Resources
```c
struct wrapper_device {
    // ... existing fields ...
    bool null_descriptors_enabled;
    
    // Dummy resources for substitution
    VkBuffer dummy_buffer;
    VkDeviceMemory dummy_buffer_memory;
    VkImage dummy_image_1d, dummy_image_2d, dummy_image_3d;
    VkDeviceMemory dummy_image_memory_1d, dummy_image_memory_2d, dummy_image_memory_3d;
    VkImageView dummy_image_view_1d, dummy_image_view_2d, dummy_image_view_3d;
    VkSampler dummy_sampler;
};
```

## Supported Descriptor Types

The emulation currently handles:
- `VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER`
- `VK_DESCRIPTOR_TYPE_STORAGE_BUFFER` 
- `VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC`
- `VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC`
- `VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE`
- `VK_DESCRIPTOR_TYPE_STORAGE_IMAGE`
- `VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER`
- `VK_DESCRIPTOR_TYPE_SAMPLER`

## Limitations

1. **Template Updates**: `vkUpdateDescriptorSetWithTemplate` currently passes through without null substitution (requires template parsing)
2. **Descriptor Buffers**: VK_EXT_descriptor_buffer is not yet emulated (noted for future work)
3. **Memory Overhead**: Small memory cost for dummy resources per device

## Testing

### Unit Tests
- `tests/test_nulldescriptor_logic.c`: Core substitution logic test
- `tests/test_nulldescriptors.py`: High-level emulation behavior test

### CI Integration
- Ubuntu CI workflow tests wrapper compilation
- Basic functionality verification
- Null descriptor logic validation

## DXVK Compatibility

This implementation specifically supports DXVK's usage patterns:
- Handles null buffer descriptors for unbound vertex/index buffers
- Manages null image descriptors for unbound textures
- Provides safe fallback resources that don't cause driver errors

## Performance Considerations

- **Minimal Overhead**: Detection only occurs during device creation
- **Lazy Allocation**: Dummy resources only created if emulation is needed
- **Zero Runtime Cost**: No additional overhead when native support is available
- **Memory Efficient**: Tiny dummy resources (1 byte buffer, 1x1 pixel images)

## Future Enhancements

1. **Template Support**: Full `vkUpdateDescriptorSetWithTemplate` emulation
2. **Descriptor Buffers**: VK_EXT_descriptor_buffer emulation
3. **Resource Pooling**: Share dummy resources across multiple devices
4. **Advanced Detection**: More sophisticated native feature detection