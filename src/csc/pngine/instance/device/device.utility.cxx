module;
#include <cstdint>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
module csc.pngine.instance.device:utility;
export import vulkan_hpp;

import stl.optional;

namespace csc {
namespace pngine {

struct queue_family_indices {
  std::optional<uint32_t> graphics = std::nullopt;
  std::optional<uint32_t> compute = std::nullopt;
  std::optional<uint32_t> transfer = std::nullopt;
};

pngine::queue_family_indices bring_indices_from_phys_device(const vk::PhysicalDevice& dev) {
  queue_family_indices indices;
  auto has_graphics_index = [](const auto& family) {
    return static_cast<bool>(family.queueFlags & vk::QueueFlagBits::eGraphics);
  };
  auto has_compute_index = [](const auto& family) {
    return static_cast<bool>(family.queueFlags & vk::QueueFlagBits::eCompute);
  };
  auto has_transfer_index = [](const auto& family) {
    return static_cast<bool>(family.queueFlags & vk::QueueFlagBits::eTransfer);
  };
  const auto& families = dev.getQueueFamilyProperties();
  const auto graphics_pos = std::ranges::find_if(families, has_graphics_index);
  const auto compute_pos = std::ranges::find_if(families, has_compute_index);
  const auto transfer_pos = std::ranges::find_if(families, has_transfer_index);
  if (graphics_pos != families.cend())
    indices.graphics = static_cast<uint32_t>(std::distance(families.cbegin(), graphics_pos));
  if (compute_pos != families.cend())
    indices.compute = static_cast<uint32_t>(std::distance(families.cbegin(), compute_pos));
  if (transfer_pos != families.cend())
    indices.transfer = static_cast<uint32_t>(std::distance(families.cbegin(), transfer_pos));
  return indices;
}

} // namespace pngine
} // namespace csc
