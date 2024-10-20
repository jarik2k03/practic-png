include(png_short_paths)

add_subdirectory(attributes)

add_library(${DEFLATER}_utility OBJECT deflater.utility.cxx)
target_sources(${DEFLATER}_utility PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES deflater.utility.cxx ) 
target_link_libraries(${DEFLATER}_utility PRIVATE ZLIB::ZLIB ${BUFFER} ${BUFFER_VIEW} stl_stdexcept)

add_library(${DEFLATER}_impl STATIC deflater.impl.cxx)
target_sources(${DEFLATER}_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES deflater.impl.cxx ) 
target_link_libraries(${DEFLATER}_impl PRIVATE ${DEFLATER}_utility ${DEFLATER_ATTRIBUTES} ${MEMORY_LITERALS})

add_library(${DEFLATER} STATIC deflater.lib.cxx)
target_sources(${DEFLATER} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES deflater.lib.cxx ) 
target_link_libraries(${DEFLATER} PRIVATE ${DEFLATER}_impl)

