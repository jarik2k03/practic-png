# макросы
set(src unfilterer)
set(target png_deserializer_unfilterer)

add_library(${target} STATIC ${src}.lib.cxx)
target_sources(${target} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES ${src}.lib.cxx )
target_link_libraries(${target} PRIVATE png_commons_buffer_view png_commons_buffer png_commons_filtering stl_stdexcept stl_vector)
