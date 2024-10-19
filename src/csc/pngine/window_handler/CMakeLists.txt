if(WIN32)
add_subdirectory(win32_handler)
elseif(LINUX)
add_subdirectory(xcb_handler)
endif()

include(pngine_short_paths)
# макросы
set(src window_handler)
set(target ${WINDOW_HANDLER})

add_library(${target}_visitors STATIC ${src}.visitors.cxx)
target_sources(${target}_visitors PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES ${src}.visitors.cxx )
if(WIN32)
  target_link_libraries(${target}_visitors PRIVATE ${WIN32_HANDLER})
elseif(LINUX)
  target_link_libraries(${target}_visitors PRIVATE ${XCB_HANDLER})
endif()

add_library(${target} STATIC ${src}.lib.cxx)
target_sources(${target} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES ${src}.lib.cxx )
target_link_libraries(${target} PRIVATE ${target}_visitors stl_variant)
