include(compile_shader)

add_library(shaders_triangle INTERFACE)
compile_shader(shaders_triangle ENV vulkan FORMAT bin SOURCES triangle.vert triangle.frag)
create_shaderpath_header(shaders_triangle)

add_library(shaders_clipping INTERFACE)
compile_shader(shaders_clipping ENV vulkan FORMAT bin SOURCES clipping.comp)
create_shaderpath_header(shaders_clipping)

add_library(shaders_scaling INTERFACE)
compile_shader(shaders_scaling ENV vulkan FORMAT bin SOURCES scaling.comp)
create_shaderpath_header(shaders_scaling)

add_library(shaders_rotating INTERFACE)
compile_shader(shaders_rotating ENV vulkan FORMAT bin SOURCES rotating.comp)
create_shaderpath_header(shaders_rotating)
