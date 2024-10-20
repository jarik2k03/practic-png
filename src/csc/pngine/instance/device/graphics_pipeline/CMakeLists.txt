include(pngine_short_paths)
# макросы
set(src graphics_pipeline)
set(target ${GRAPHICS_PIPELINE})

add_library(${target} STATIC ${src}.lib.cxx)
target_sources(${target} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES ${src}.lib.cxx )
target_link_libraries(${target} PRIVATE vk_vulkan stl_vector stl_array ${UTILITY}_graphics_pipeline)
