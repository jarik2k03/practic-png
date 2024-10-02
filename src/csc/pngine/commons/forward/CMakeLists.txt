include(pngine_short_paths)
add_library(${FORWARD}_device STATIC device.fwd.cxx)
target_sources(${FORWARD}_device PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES device.fwd.cxx )
