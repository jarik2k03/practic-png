include(pngine_short_paths)


# add_subdirectory(surfaceKHR)
add_subdirectory(debug_reportEXT)
add_subdirectory(device)
add_subdirectory(surfaceKHR)

add_library(${INSTANCE} STATIC instance.lib.cxx)
target_sources(${INSTANCE} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES instance.lib.cxx )
target_link_libraries(${INSTANCE} PRIVATE vk_vulkan  stl_set stl_string_view stl_string ${DEVICE} ${DEBUG_REPORTEXT} ${SURFACEKHR})
