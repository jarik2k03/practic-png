include(pngine_short_paths)
# макросы
set(src fence)
set(target ${FENCE})

add_library(${target}_impl OBJECT ${src}.impl.cxx)
target_sources(${target}_impl PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES ${src}.impl.cxx )
target_link_libraries(${target}_impl PRIVATE vk_vulkan) 

add_library(${target} STATIC ${src}.lib.cxx)
target_sources(${target} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES ${src}.lib.cxx )
target_link_libraries(${target} PRIVATE ${target}_impl)


