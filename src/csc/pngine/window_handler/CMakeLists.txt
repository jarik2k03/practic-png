include(pngine_short_paths)
# макросы
set(src window_handler)
set(target ${WINDOW_HANDLER})

add_library(${target} STATIC ${src}.lib.cxx)
target_sources(${target} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES ${src}.lib.cxx )
target_link_libraries(${target} PRIVATE glfw stl_stdexcept stl_string_view)

# макросы
set(src glfw_handler)
set(target ${GLFW_HANDLER})

add_library(${target} STATIC ${src}.lib.cxx)
target_sources(${target} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES ${src}.lib.cxx )
target_link_libraries(${target} PRIVATE glfw)
