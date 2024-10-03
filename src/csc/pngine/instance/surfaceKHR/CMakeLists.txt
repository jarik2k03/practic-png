include(pngine_short_paths)

add_library(${SURFACEKHR}_utility OBJECT surfaceKHR.utility.cxx)
target_sources(${SURFACEKHR}_utility PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES surfaceKHR.utility.cxx)
target_link_libraries(${SURFACEKHR}_utility PRIVATE vk_vulkan stl_optional)

add_library(${SURFACEKHR}_impl OBJECT surfaceKHR.impl.cxx)
target_sources(${SURFACEKHR}_impl PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES surfaceKHR.impl.cxx )
target_link_libraries(${SURFACEKHR}_impl PRIVATE ${SURFACEKHR}_utility)

add_library(${SURFACEKHR} STATIC surfaceKHR.lib.cxx)
target_sources(${SURFACEKHR} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES surfaceKHR.lib.cxx )
target_link_libraries(${SURFACEKHR} PRIVATE ${SURFACEKHR}_impl)

