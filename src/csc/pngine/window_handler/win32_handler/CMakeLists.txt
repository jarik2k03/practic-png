include(pngine_short_paths)
add_library(${WIN32_HANDLER}_impl STATIC win32_handler.impl.cxx)
target_sources(${WIN32_HANDLER}_impl PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES win32_handler.impl.cxx )

add_library(${WIN32_HANDLER} STATIC win32_handler.lib.cxx)
target_sources(${WIN32_HANDLER} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES win32_handler.lib.cxx )
target_link_libraries(${WIN32_HANDLER} PRIVATE ${WIN32_HANDLER}_impl)


