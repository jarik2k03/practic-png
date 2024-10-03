include(pngine_short_paths)

add_library(${IMAGE_VIEW}_utility OBJECT image_view.utility.cxx)
target_sources(${IMAGE_VIEW}_utility PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES image_view.utility.cxx )
target_link_libraries(${IMAGE_VIEW}_utility PRIVATE vk_vulkan)

add_library(${IMAGE_VIEW}_impl OBJECT image_view.impl.cxx)
target_sources(${IMAGE_VIEW}_impl PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES image_view.impl.cxx )
target_link_libraries(${IMAGE_VIEW}_impl PRIVATE ${IMAGE_VIEW}_utility stl_vector)

add_library(${IMAGE_VIEW} STATIC image_view.lib.cxx)
target_sources(${IMAGE_VIEW} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES image_view.lib.cxx )
target_link_libraries(${IMAGE_VIEW} PRIVATE ${IMAGE_VIEW}_impl)


