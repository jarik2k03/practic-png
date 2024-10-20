# Require Vulkan version ≥ 1.3.256 (earliest version when the Vulkan module was available)
if( ${Vulkan_VERSION} VERSION_LESS "1.3.256" )
  message( FATAL_ERROR "Minimum required Vulkan version for C++ modules is 1.3.256. "
           "Found ${Vulkan_VERSION}.")
endif()
add_library(vk_vulkan STATIC)
target_sources(vk_vulkan PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${Vulkan_INCLUDE_DIR} FILES ${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm)
target_compile_features(vk_vulkan PUBLIC cxx_std_20 )
target_link_libraries(vk_vulkan PUBLIC Vulkan::Vulkan)

