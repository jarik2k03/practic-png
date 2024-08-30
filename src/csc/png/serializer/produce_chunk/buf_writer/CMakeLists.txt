add_library(buf_writer_attributes STATIC buf_writer.attributes.cxx)
target_sources(buf_writer_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buf_writer.attributes.cxx) 
target_link_libraries(buf_writer_attributes PRIVATE utility)

add_library(buf_writer_impl STATIC buf_writer.impl.cxx)
target_sources(buf_writer_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buf_writer.impl.cxx ) 
target_link_libraries(buf_writer_impl PRIVATE buf_writer_attributes)

add_library(buf_writer STATIC buf_writer.lib.cxx)
target_sources(buf_writer PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buf_writer.lib.cxx ) 
target_link_libraries(buf_writer PRIVATE buf_writer_impl)



