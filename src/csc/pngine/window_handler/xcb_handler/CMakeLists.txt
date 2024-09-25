include(pngine_short_paths)
add_library(${XCB_HANDLER}_utility STATIC xcb_handler.utility.cxx)
target_sources(${XCB_HANDLER}_utility PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES xcb_handler.utility.cxx )

add_library(${XCB_HANDLER}_impl STATIC xcb_handler.impl.cxx)
target_sources(${XCB_HANDLER}_impl PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES xcb_handler.impl.cxx )
target_link_libraries(${XCB_HANDLER}_impl PRIVATE ${XCB_HANDLER}_utility X11::xcb stl_stdexcept)

add_library(${XCB_HANDLER} STATIC xcb_handler.lib.cxx)
target_sources(${XCB_HANDLER} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES xcb_handler.lib.cxx )
target_link_libraries(${XCB_HANDLER} PRIVATE ${XCB_HANDLER}_impl)

