#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexcoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler image_sampler;
layout(set = 1, binding = 1) uniform texture2D image_texture;


void main() {
    outColor = texture(sampler2D(image_texture, image_sampler), fragTexcoord); // - стандартный rgba
}
