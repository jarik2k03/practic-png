module;
#include <vulkan/vulkan_core.h>
module csc.pngine.instance.debug_reportEXT:utility;
export import vulkan_hpp;

namespace csc {
namespace pngine {

using pfn_create = PFN_vkCreateDebugReportCallbackEXT;
using pfn_destroy = PFN_vkDestroyDebugReportCallbackEXT;
struct debug_dispatch : public vk::DispatchLoaderBase {
  pfn_create vkCreateDebugReportCallbackEXT = 0ul;
  pfn_destroy vkDestroyDebugReportCallbackEXT = 0ul;
};

} // namespace pngine
} // namespace csc
