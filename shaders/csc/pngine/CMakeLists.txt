include(compile_shader)

add_library(shaders_triangle INTERFACE)
compile_shader(shaders_triangle ENV vulkan FORMAT bin SOURCES triangle.vert triangle.frag)
create_shaderpath_header(shaders_triangle)
