include(pngine_short_paths)

add_library(${POLL_EVENT} STATIC poll_event.lib.cxx)
target_sources(${POLL_EVENT} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES poll_event.lib.cxx )
if(WIN32)
  target_link_libraries(${POLL_EVENT} PRIVATE ${WIN32_HANDLER})
elseif(LINUX)
  target_link_libraries(${POLL_EVENT} PRIVATE ${XCB_HANDLER})
endif()


