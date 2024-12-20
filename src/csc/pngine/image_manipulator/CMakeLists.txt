# макросы
set(src image_manipulator)
set(target pngine_image_manipulator)

add_library(${target}_shaderdata STATIC ${src}.shaderdata.cxx)
target_sources(${target}_shaderdata PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES ${src}.shaderdata.cxx)
target_link_libraries(${target}_shaderdata PRIVATE glm)

add_library(${target} STATIC ${src}.lib.cxx)
target_sources(${target} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES ${src}.lib.cxx )
target_link_libraries(${target} PRIVATE vk_vulkan stl_stdexcept stl_vector stl_array pngine_instance_device ${target}_shaderdata)

target_link_libraries(${target} INTERFACE shaders_clipping)
target_include_directories(${target} PRIVATE ${CMAKE_BINARY_DIR}/shaders/csc/pngine)
