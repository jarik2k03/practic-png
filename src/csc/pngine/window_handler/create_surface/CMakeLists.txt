include(pngine_short_paths)

add_library(${CREATE_SURFACE} STATIC create_surface.lib.cxx)
target_sources(${CREATE_SURFACE} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES create_surface.lib.cxx )
target_link_libraries(${CREATE_SURFACE} PRIVATE vk_vulkan)
if(WIN32)
  target_link_libraries(${CREATE_SURFACE} PRIVATE ${WIN32_HANDLER})
elseif(LINUX)
  target_link_libraries(${CREATE_SURFACE} PRIVATE ${XCB_HANDLER})
endif()

