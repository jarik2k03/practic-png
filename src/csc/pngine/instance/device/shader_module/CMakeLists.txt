include(pngine_short_paths)

add_library(${SHADER_MODULE} STATIC shader_module.lib.cxx)
target_sources(${SHADER_MODULE} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES shader_module.lib.cxx )
target_link_libraries(${SHADER_MODULE} PRIVATE ${UTILITY}_shader_module)


