add_subdirectory(commons)
add_subdirectory(instance)
add_subdirectory(window_handler)
include(pngine_short_paths)

add_library(pngine_attributes STATIC pngine.attributes.cxx)
target_sources(pngine_attributes PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES pngine.attributes.cxx)
target_link_libraries(pngine_attributes PRIVATE ${UTILITY}_version ${WINDOW_HANDLER} stl_optional)


add_library(pngine_impl STATIC pngine.impl.cxx)
target_include_directories(pngine_impl PRIVATE ${CMAKE_BINARY_DIR}/shaders/csc/pngine)
target_link_libraries(pngine_impl INTERFACE shaders_triangle) # теперь нам доступен сгенерированный заголовок с путями к шейдеру!

target_sources(pngine_impl PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES pngine.impl.cxx )
target_link_libraries(pngine_impl PRIVATE pngine_attributes ${INSTANCE} stl_string stl_string_view)

add_library(pngine STATIC pngine.lib.cxx)
target_sources(pngine PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES pngine.lib.cxx )
target_link_libraries(pngine PRIVATE pngine_impl)

