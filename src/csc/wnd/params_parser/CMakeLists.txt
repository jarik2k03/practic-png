set(src params_parser)
set(target wnd_params_parser)

add_library(${target} STATIC ${src}.lib.cxx)
target_sources(${target} PUBLIC FILE_SET csc_wnd TYPE CXX_MODULES FILES ${src}.lib.cxx )
target_link_libraries(${target} PRIVATE glm stl_string stl_stringstream stl_iostream)

