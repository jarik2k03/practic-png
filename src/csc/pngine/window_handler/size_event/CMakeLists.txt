include(pngine_short_paths)

add_library(${SIZE_EVENT} STATIC size_event.lib.cxx)
target_sources(${SIZE_EVENT} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES size_event.lib.cxx )
if(WIN32)
  target_link_libraries(${SIZE_EVENT} PRIVATE ${WIN32_HANDLER})
elseif(LINUX)
  target_link_libraries(${SIZE_EVENT} PRIVATE ${XCB_HANDLER})
endif()



