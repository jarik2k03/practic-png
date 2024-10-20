include(pngine_short_paths)

set(src graphics_pipeline)
set(target ${UTILITY}_graphics_pipeline)

add_library(${target} STATIC ${src}.lib.cxx)
target_sources(${target} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES ${src}.lib.cxx )
target_link_libraries(${target} PRIVATE vk_vulkan stl_vector stl_string)


