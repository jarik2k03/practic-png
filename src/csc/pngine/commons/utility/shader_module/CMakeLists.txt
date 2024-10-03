include(pngine_short_paths)
add_library(${UTILITY}_shader_module STATIC shader_module.lib.cxx)
target_sources(${UTILITY}_shader_module PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES shader_module.lib.cxx )
target_link_libraries(${UTILITY}_shader_module PRIVATE vk_vulkan stl_vector stl_string_view stl_fstream stl_stdexcept stl_string stl_ios)

