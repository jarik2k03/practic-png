include(pngine_short_paths)

add_library(${XCB_HANDLER} STATIC xcb_handler.lib.cxx)
target_sources(${XCB_HANDLER} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES xcb_handler.lib.cxx )
target_link_libraries(${XCB_HANDLER} PRIVATE X11::xcb stl_stdexcept stl_string_view vk_vulkan)

