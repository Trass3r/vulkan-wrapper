#include "wrapper_bc.h"
#include "wrapper_private.h"

/* Define BCDEC_IMPLEMENTATION to include implementation */
#define BCDEC_IMPLEMENTATION
#include "bcdec.h"

#include "vk_log.h"
#include "util/macros.h"

#include <string.h>
#include <assert.h>

/* BC format lookup table */
static const struct bc_format_info bc_formats[] = {
   {
      .format = VK_FORMAT_BC1_RGB_UNORM_BLOCK,
      .block_size = BCDEC_BC1_BLOCK_SIZE,
      .block_width = 4,
      .block_height = 4,
      .decompressed_format = VK_FORMAT_R8G8B8A8_UNORM,
      .name = "BC1_RGB_UNORM"
   },
   {
      .format = VK_FORMAT_BC1_RGB_SRGB_BLOCK,
      .block_size = BCDEC_BC1_BLOCK_SIZE,
      .block_width = 4,
      .block_height = 4,
      .decompressed_format = VK_FORMAT_R8G8B8A8_SRGB,
      .name = "BC1_RGB_SRGB"
   },
   {
      .format = VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
      .block_size = BCDEC_BC1_BLOCK_SIZE,
      .block_width = 4,
      .block_height = 4,
      .decompressed_format = VK_FORMAT_R8G8B8A8_UNORM,
      .name = "BC1_RGBA_UNORM"
   },
   {
      .format = VK_FORMAT_BC1_RGBA_SRGB_BLOCK,
      .block_size = BCDEC_BC1_BLOCK_SIZE,
      .block_width = 4,
      .block_height = 4,
      .decompressed_format = VK_FORMAT_R8G8B8A8_SRGB,
      .name = "BC1_RGBA_SRGB"
   },
};

static const uint32_t bc_format_count = ARRAY_SIZE(bc_formats);

bool wrapper_is_bc_format(VkFormat format)
{
   for (uint32_t i = 0; i < bc_format_count; i++) {
      if (bc_formats[i].format == format)
         return true;
   }
   return false;
}

const struct bc_format_info* wrapper_get_bc_format_info(VkFormat format)
{
   for (uint32_t i = 0; i < bc_format_count; i++) {
      if (bc_formats[i].format == format)
         return &bc_formats[i];
   }
   return NULL;
}

VkDeviceSize wrapper_bc_get_compressed_size(VkFormat format, uint32_t width, uint32_t height)
{
   const struct bc_format_info *info = wrapper_get_bc_format_info(format);
   if (!info)
      return 0;

   /* Calculate number of blocks */
   uint32_t blocks_x = (width + info->block_width - 1) / info->block_width;
   uint32_t blocks_y = (height + info->block_height - 1) / info->block_height;
   
   return blocks_x * blocks_y * info->block_size;
}

static VkResult 
wrapper_bc1_decompress_block(const void *compressed_block, 
                            void *decompressed_block,
                            uint32_t block_pitch)
{
   /* Use bcdec to decompress BC1 block to RGBA8 */
   bcdec_bc1(compressed_block, decompressed_block, block_pitch);
   return VK_SUCCESS;
}

VkResult wrapper_bc_decompress_image(struct wrapper_device *device,
                                   VkFormat src_format,
                                   uint32_t width,
                                   uint32_t height,
                                   const void *compressed_data,
                                   VkDeviceSize compressed_size,
                                   void *decompressed_data,
                                   VkDeviceSize decompressed_size)
{
   const struct bc_format_info *info = wrapper_get_bc_format_info(src_format);
   if (!info) {
      vk_loge(VK_LOG_OBJS(&device->vk.base), 
              "Unsupported BC format for emulation: %d", src_format);
      return VK_ERROR_FORMAT_NOT_SUPPORTED;
   }

   /* Calculate block dimensions */
   uint32_t blocks_x = (width + info->block_width - 1) / info->block_width;
   uint32_t blocks_y = (height + info->block_height - 1) / info->block_height;
   
   /* Verify size consistency */
   VkDeviceSize expected_compressed_size = blocks_x * blocks_y * info->block_size;
   if (compressed_size < expected_compressed_size) {
      vk_loge(VK_LOG_OBJS(&device->vk.base),
              "BC decompression: insufficient compressed data size");
      return VK_ERROR_UNKNOWN;
   }

   /* For now, implement BC1 only as requested */
   if (src_format != VK_FORMAT_BC1_RGB_UNORM_BLOCK &&
       src_format != VK_FORMAT_BC1_RGB_SRGB_BLOCK &&
       src_format != VK_FORMAT_BC1_RGBA_UNORM_BLOCK &&
       src_format != VK_FORMAT_BC1_RGBA_SRGB_BLOCK) {
      vk_loge(VK_LOG_OBJS(&device->vk.base),
              "BC format %s not yet implemented", info->name);
      return VK_ERROR_FEATURE_NOT_PRESENT;
   }

   /* Decompress each 4x4 block */
   const uint8_t *src_blocks = (const uint8_t *)compressed_data;
   uint8_t *dst_image = (uint8_t *)decompressed_data;
   
   /* Calculate destination pitch (width * 4 bytes per RGBA pixel) */
   uint32_t dst_pitch = width * 4;
   
   for (uint32_t block_y = 0; block_y < blocks_y; block_y++) {
      for (uint32_t block_x = 0; block_x < blocks_x; block_x++) {
         /* Calculate source block pointer */
         const uint8_t *src_block = src_blocks + 
            (block_y * blocks_x + block_x) * info->block_size;
         
         /* Calculate destination block pointer */
         uint8_t *dst_block = dst_image + 
            (block_y * info->block_height * dst_pitch) + 
            (block_x * info->block_width * 4);

         /* Decompress this block */
         VkResult result = wrapper_bc1_decompress_block(src_block, dst_block, dst_pitch);
         if (result != VK_SUCCESS) {
            vk_loge(VK_LOG_OBJS(&device->vk.base),
                    "Failed to decompress BC1 block at (%u, %u)", block_x, block_y);
            return result;
         }
      }
   }

   vk_logi(VK_LOG_OBJS(&device->vk.base),
           "Successfully decompressed %s image (%ux%u, %u blocks)",
           info->name, width, height, blocks_x * blocks_y);

   return VK_SUCCESS;
}

VkResult wrapper_bc_device_init(struct wrapper_device *device)
{
   /* BC emulation initialization - currently no device-specific setup needed */
   vk_logi(VK_LOG_OBJS(&device->vk.base), 
           "BC texture compression emulation initialized");
   return VK_SUCCESS;
}

void wrapper_bc_device_finish(struct wrapper_device *device)
{
   /* BC emulation cleanup - currently no cleanup needed */
   vk_logi(VK_LOG_OBJS(&device->vk.base),
           "BC texture compression emulation finalized");
}