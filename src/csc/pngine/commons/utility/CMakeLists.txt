add_subdirectory(version)
add_subdirectory(debug_reportEXT)
add_subdirectory(swapchain_details)
add_subdirectory(queue_family_indices)

include(pngine_short_paths)
add_library(${UTILITY} STATIC utility.lib.cxx)
target_sources(${UTILITY} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES utility.lib.cxx )
target_link_libraries(${UTILITY} PRIVATE ${UTILITY}_version ${UTILITY}_debug_reportEXT ${UTILITY}_swapchain_details ${UTILITY}_queue_family_indices)

