include(pngine_short_paths)
# макросы
set(src render_pass)
set(target ${RENDER_PASS})

add_library(${target} STATIC ${src}.lib.cxx)
target_sources(${target} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES ${src}.lib.cxx )
target_link_libraries(${target} PRIVATE vk_vulkan)


