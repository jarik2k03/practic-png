include(png_short_paths)

add_library(${BUF_WRITER}_attributes STATIC buf_writer.attributes.cxx)
target_sources(${BUF_WRITER}_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buf_writer.attributes.cxx) 
target_link_libraries(${BUF_WRITER}_attributes PRIVATE ${ENDIAN})

add_library(${BUF_WRITER}_impl STATIC buf_writer.impl.cxx)
target_sources(${BUF_WRITER}_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buf_writer.impl.cxx ) 
target_link_libraries(${BUF_WRITER}_impl PRIVATE ${BUF_WRITER}_attributes)

add_library(${BUF_WRITER} STATIC buf_writer.lib.cxx)
target_sources(${BUF_WRITER} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buf_writer.lib.cxx ) 
target_link_libraries(${BUF_WRITER} PRIVATE ${BUF_WRITER}_impl)



