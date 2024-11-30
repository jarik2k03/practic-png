add_subdirectory(commons)
add_subdirectory(instance)
add_subdirectory(window_handler)
add_subdirectory(vertex)
include(pngine_short_paths)

add_library(pngine_attributes STATIC pngine.attributes.cxx)
target_sources(pngine_attributes PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES pngine.attributes.cxx)
target_link_libraries(pngine_attributes PRIVATE ${UTILITY}_version)

add_library(pngine STATIC pngine.lib.cxx)
target_sources(pngine PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES pngine.lib.cxx )
target_link_libraries(pngine PRIVATE pngine_attributes ${INSTANCE} ${WINDOW_HANDLER} ${GLFW_HANDLER} ${VERTEX} png_picture_sections_IHDR)
# подключаем сгенерированный заголовок
target_include_directories(pngine PRIVATE ${CMAKE_BINARY_DIR}/shaders/csc/pngine)
target_link_libraries(pngine INTERFACE shaders_triangle) # теперь нам доступен сгенерированный заголовок с путями к шейдеру!
