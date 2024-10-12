include(pngine_short_paths)
# макросы
set(src render_pass)
set(target ${RENDER_PASS})

add_library(${target}_utility OBJECT ${src}.utility.cxx)
target_sources(${target}_utility PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES ${src}.utility.cxx )
target_link_libraries(${target}_utility PRIVATE vk_vulkan)

add_library(${target}_impl OBJECT ${src}.impl.cxx)
target_sources(${target}_impl PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES ${src}.impl.cxx )
target_link_libraries(${target}_impl PRIVATE ${target}_utility stl_vector)

add_library(${target} STATIC ${src}.lib.cxx)
target_sources(${target} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES ${src}.lib.cxx )
target_link_libraries(${target} PRIVATE ${target}_impl)


