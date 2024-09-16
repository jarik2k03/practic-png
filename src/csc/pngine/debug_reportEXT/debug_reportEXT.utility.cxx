module;
#include <cstdint>
#include <cstdlib>
#include <bits/stl_iterator.h>
#include <vulkan/vk_platform.h>
module csc.pngine.debug_reportEXT:utility;
export import vulkan_hpp;

#ifndef NDEBUG
import stl.iostream;
#endif

namespace csc {
namespace pngine {

VKAPI_ATTR vk::Bool32 VKAPI_CALL custom_debug_report_callback(
    vk::DebugReportFlagsEXT /*flags*/,
    vk::DebugReportObjectTypeEXT /*objectType*/,
    uint64_t /*object*/,
    size_t /*location*/,
    int32_t /*messageCode*/,
    const char* /*pLayerPrefix*/,
    const char* /*pMessage*/,
    void* /*pUserData*/) {
#ifndef NDEBUG
  // std::clog << "debug report: " << pMessage << '\n';
#endif
  return 0;
}
} // namespace pngine
} // namespace csc
