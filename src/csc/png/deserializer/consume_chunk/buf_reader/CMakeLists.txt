add_library(buf_reader_attributes STATIC buf_reader.attributes.cxx)
target_sources(buf_reader_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buf_reader.attributes.cxx) 
target_link_libraries(buf_reader_attributes PRIVATE utility)

add_library(buf_reader_impl STATIC buf_reader.impl.cxx)
target_sources(buf_reader_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buf_reader.impl.cxx ) 
target_link_libraries(buf_reader_impl PRIVATE buf_reader_attributes)

add_library(buf_reader STATIC buf_reader.lib.cxx)
target_sources(buf_reader PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buf_reader.lib.cxx ) 
target_link_libraries(buf_reader PRIVATE buf_reader_impl)


