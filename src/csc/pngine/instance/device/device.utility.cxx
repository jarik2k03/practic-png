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
  std::optional<uint32_t> present = std::nullopt;
  std::optional<uint32_t> compute = std::nullopt;
  std::optional<uint32_t> transfer = std::nullopt;
};

pngine::queue_family_indices bring_indices_from_phys_device(const vk::PhysicalDevice& dev) {
  const auto& families = dev.getQueueFamilyProperties();
  auto has_graphics_index = [](const auto& family) {
    return static_cast<bool>(family.queueFlags & vk::QueueFlagBits::eGraphics);
  };
  auto has_compute_index = [](const auto& family) {
    return static_cast<bool>(family.queueFlags & vk::QueueFlagBits::eCompute);
  };
  auto has_transfer_index = [](const auto& family) {
    return static_cast<bool>(family.queueFlags & vk::QueueFlagBits::eTransfer);
  };
  const auto graphics_pos = std::ranges::find_if(families, has_graphics_index);
  const auto compute_pos = std::ranges::find_if(families, has_compute_index);
  const auto transfer_pos = std::ranges::find_if(families, has_transfer_index);
  queue_family_indices indices;
  if (graphics_pos != families.cend())
    indices.graphics = static_cast<uint32_t>(std::distance(families.cbegin(), graphics_pos));
  if (compute_pos != families.cend())
    indices.compute = static_cast<uint32_t>(std::distance(families.cbegin(), compute_pos));
  if (transfer_pos != families.cend())
    indices.transfer = static_cast<uint32_t>(std::distance(families.cbegin(), transfer_pos));
  return indices;
}

pngine::queue_family_indices bring_indices_from_phys_device(
    const vk::PhysicalDevice& dev,
    const vk::SurfaceKHR& surface) {
  const auto& families = dev.getQueueFamilyProperties();
  std::optional<uint32_t> present_pos = std::nullopt; // "пустой" индекс
  for (uint32_t index = 0u; index < families.size(); ++index) {
    if (dev.getSurfaceSupportKHR(index, surface) == true) {
      present_pos = std::make_optional(index);
      break;
    }
  }
  auto indices = pngine::bring_indices_from_phys_device(dev);
  if (present_pos)
    indices.present = (present_pos.has_value()) ? present_pos : std::nullopt;
  return indices;
}

} // namespace pngine
} // namespace csc
