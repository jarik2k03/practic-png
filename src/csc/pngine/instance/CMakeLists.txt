include(pngine_short_paths)


# add_subdirectory(surfaceKHR)
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
add_subdirectory(debug_reportEXT)
endif()
add_subdirectory(device)

add_library(${INSTANCE}_utility STATIC instance.utility.cxx)
target_sources(${INSTANCE}_utility PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES instance.utility.cxx)
target_link_libraries(${INSTANCE}_utility PRIVATE vk_vulkan stl_vector stl_string stl_ostream)

add_library(${INSTANCE}_impl STATIC instance.impl.cxx)
target_sources(${INSTANCE}_impl PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES instance.impl.cxx )
target_link_libraries(${INSTANCE}_impl PRIVATE ${INSTANCE}_utility ${DEVICE} stl_set)
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  target_link_libraries(${INSTANCE}_impl PRIVATE ${DEBUG_REPORTEXT})
endif()

add_library(${INSTANCE} STATIC instance.lib.cxx)
target_sources(${INSTANCE} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES instance.lib.cxx )
target_link_libraries(${INSTANCE} PRIVATE ${INSTANCE}_impl)

