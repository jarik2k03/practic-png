add_subdirectory(controller)
add_subdirectory(window_handler)
add_subdirectory(params_parser)

set(src wnd)
set(target wnd)

add_library(${target} STATIC ${src}.lib.cxx)
target_sources(${target} PUBLIC FILE_SET csc_wnd TYPE CXX_MODULES FILES ${src}.lib.cxx )
target_link_libraries(${target} PRIVATE wnd_window_handler wnd_glfw_handler wnd_controller wnd_params_parser pngine png_picture stl_iostream)
