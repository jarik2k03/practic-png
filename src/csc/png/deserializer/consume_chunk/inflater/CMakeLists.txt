include(png_short_paths)

add_library(${INFLATER}_utility OBJECT inflater.utility.cxx)
target_sources(${INFLATER}_utility PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES inflater.utility.cxx )
target_link_libraries(${INFLATER}_utility PRIVATE ${BUFFER} ${BUFFER_VIEW} stl_stdexcept ZLIB::ZLIB)

add_library(${INFLATER}_impl STATIC inflater.impl.cxx)
target_sources(${INFLATER}_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES inflater.impl.cxx )
target_link_libraries(${INFLATER}_impl PRIVATE ${INFLATER}_utility ${MEMORY_LITERALS})

add_library(${INFLATER} STATIC inflater.lib.cxx)
target_sources(${INFLATER} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES inflater.lib.cxx )
target_link_libraries(${INFLATER} PRIVATE ${INFLATER}_impl)
