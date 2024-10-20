include(png_short_paths)

add_library(${BUF_READER}_attributes STATIC buf_reader.attributes.cxx)
target_sources(${BUF_READER}_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buf_reader.attributes.cxx) 
target_link_libraries(${BUF_READER}_attributes PRIVATE ${ENDIAN})

add_library(${BUF_READER}_impl STATIC buf_reader.impl.cxx)
target_sources(${BUF_READER}_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buf_reader.impl.cxx ) 
target_link_libraries(${BUF_READER}_impl PRIVATE ${BUF_READER}_attributes)

add_library(${BUF_READER} STATIC buf_reader.lib.cxx)
target_sources(${BUF_READER} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buf_reader.lib.cxx ) 
target_link_libraries(${BUF_READER} PRIVATE ${BUF_READER}_impl)


