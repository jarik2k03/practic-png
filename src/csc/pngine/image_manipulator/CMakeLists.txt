# макросы
set(src image_manipulator)
set(target pngine_image_manipulator)

add_library(${target} STATIC ${src}.lib.cxx)
target_sources(${target} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES ${src}.lib.cxx )
target_link_libraries(${target} PRIVATE vk_vulkan stl_stdexcept stl_vector stl_array pngine_instance_device pngine_commons_toolbox_params)

target_link_libraries(${target} INTERFACE shaders_clipping shaders_scaling shaders_rotating)
target_include_directories(${target} PRIVATE ${CMAKE_BINARY_DIR}/shaders/csc/pngine)
