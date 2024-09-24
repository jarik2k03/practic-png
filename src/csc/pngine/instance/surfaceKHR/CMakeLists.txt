include(pngine_short_paths)

add_library(${SURFACEKHR}_utility STATIC device.utility.cxx)
target_sources(${SURFACEKHR}_utility PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES device.utility.cxx)
target_link_libraries(${SURFACEKHR}_utility PRIVATE vk_vulkan stl_optional)

add_library(${SURFACEKHR}_impl STATIC device.impl.cxx)
target_sources(${SURFACEKHR}_impl PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES device.impl.cxx )
target_link_libraries(${SURFACEKHR}_impl PRIVATE ${SURFACEKHR}_utility stl_stdexcept stl_vector)

add_library(${SURFACEKHR} STATIC device.lib.cxx)
target_sources(${SURFACEKHR} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES device.lib.cxx )
target_link_libraries(${SURFACEKHR} PRIVATE ${SURFACEKHR}_impl)

