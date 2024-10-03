include(pngine_short_paths)

add_library(${SHADER_MODULE}_utility OBJECT shader_module.utility.cxx)
target_sources(${SHADER_MODULE}_utility PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES shader_module.utility.cxx )
target_link_libraries(${SHADER_MODULE}_utility PRIVATE vk_vulkan)

add_library(${SHADER_MODULE}_impl OBJECT shader_module.impl.cxx)
target_sources(${SHADER_MODULE}_impl PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES shader_module.impl.cxx )
target_link_libraries(${SHADER_MODULE}_impl PRIVATE ${SHADER_MODULE}_utility ${UTILITY}_shader_module)

add_library(${SHADER_MODULE} STATIC shader_module.lib.cxx)
target_sources(${SHADER_MODULE} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES shader_module.lib.cxx )
target_link_libraries(${SHADER_MODULE} PRIVATE ${SHADER_MODULE}_impl)


